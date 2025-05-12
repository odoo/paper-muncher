import asyncio
import logging
from typing import Any, Union, Tuple, Dict, Optional, overload
from contextlib import asynccontextmanager
from .communication import PaperMuncherRequest
from .default import DefaultEnvironment
from .exceptions import PaperMuncherError, PaperMuncherException
from .utils import locate_executable
from .options import PMOptions
from ._internal import SyncStreamWrapper


_logger = logging.getLogger(__name__)

@overload
def paper_muncher(
    doc : Optional[Any] = None,
    auto : bool = True,
    environment : Optional[Any] = None,
    as_stream : bool = True,
    **rendering_options : Dict[str, str],
) -> SyncStreamWrapper:
    ...
@overload
def paper_muncher(
    doc : Optional[Any] = None,
    auto : bool = True,
    environment : Optional[Any] = None,
    as_stream : bool = False,
    **rendering_options : Dict[str, str],
) -> bytes:
    ...
@overload
def paper_muncher(
    doc : Optional[Any] = None,
    auto : bool = False,
    environment : Optional[Any] = None,
    as_stream : bool = True or False,
    **rendering_options : Dict[str, str],
) -> Tuple[asyncio.subprocess.PIPE, asyncio.subprocess.PIPE, asyncio.subprocess.PIPE]:
    ...


@asynccontextmanager
async def paper_muncher(
    doc : Optional[Any] = None,
    environment : Optional[Any] = None,
    auto : bool = True,
    as_stream  : bool = True,
    **rendering_options : Dict[str, str],
) -> Union[
    SyncStreamWrapper,
    bytes,
    Tuple[asyncio.subprocess.PIPE, asyncio.subprocess.PIPE, asyncio.subprocess.PIPE],
]:
    """
    Asynchronous context manager for rendering documents using Paper Muncher.
    This function handles the rendering process and provides an interface
    for interacting with the rendering options.
    Args:
        doc (Any): The document to be rendered.
        auto (bool): Flag to indicate automatic rendering.
        environment (Optional[Any]): An optional environment object for
            handling URLs and assets.
        as_stream (bool): Flag to indicate if the output should be a stream.
        **rendering_options (dict[str, str]): Additional rendering options.
    Yields:
        Union[
            SyncStreamWrapper,
            bytes,
            Tuple[asyncio.subprocess.PIPE, asyncio.subprocess.PIPE, asyncio.subprocess.PIPE]
        ]: - the set of pipes to interact with the process if auto is False
           If the document should be rendered in pipe:     
           - the rendered document as bytes if auto is True and as_stream is False
           - a stream wrapper if auto is True and as_stream is True
           If at url or file path:
           - The path/url of the file if auto is True and as_stream is False
           - a stream wrapper if auto is True and as_stream is True
    """

    pm_options = PMOptions(**rendering_options, auto=auto)
    pm_process = await asyncio.subprocess.create_subprocess_exec(
        locate_executable(),
        *pm_options.args,
        stdin=asyncio.subprocess.PIPE,
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.PIPE,
    )

    try:
        stdin, stdout, stderr = pm_process.stdin, pm_process.stdout, pm_process.stderr
        if not auto:
            yield stdin, stdout, stderr
        else:
            if pm_options.is_piped and environment is None:
                if doc is None:
                    raise ValueError("Document cannot be None when auto is True")
            if environment is None:
                _logger.warning("No environment provided, assets will not be loaded")
                environment = DefaultEnvironment(from_doc=doc)
            while True:
                request = PaperMuncherRequest()
                await request.read_header(stdout)
                if request is None:
                    break

                response = await environment.handle_request(request)
                for chunk in response:
                    stdin.write(chunk)
                await stdin.drain()

            if as_stream:
                yield SyncStreamWrapper(stdout) 
            else:
                yield await stdout.read()

    except asyncio.CancelledError:
        _logger.warning("Paper Muncher process cancelled")
        raise
    except PaperMuncherException:
        raise
    except Exception as e:
        raise PaperMuncherError(f"Error in Paper Muncher: {e}") from e
    finally:
        pm_process.terminate()
        await pm_process.wait()
        if pm_process.returncode:
            _logger.error("Paper Muncher exited with code %s", pm_process.returncode)
