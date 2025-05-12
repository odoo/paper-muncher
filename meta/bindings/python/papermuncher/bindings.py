import logging
import sys
import os
import shutil
import subprocess
import traceback
import urllib.parse
import re


from functools import partialmethod
from typing import Any, Dict, Iterable, List, Union, Optional

from . import Environ, Request, Response, PaperMuncherException


# logging
logging.basicConfig(
    level=logging.ERROR,
    format='%(asctime)s [%(levelname)s] %(name)s: %(message)s (%(filename)s:%(lineno)d)',
    handlers=[logging.StreamHandler(sys.stdout)]
)

logger = logging.getLogger(__name__)

def exception_handler(exc_type, exc_value, exc_tb):
    exc_info = ''.join(traceback.format_exception(exc_type, exc_value, exc_tb))
    logger.error("Unhandled Exception:\n%s", exc_info)

sys.excepthook = exception_handler

# Paper Muncher constants
PAPER_MUNCHER_ALLOWED_KWARGS = {'orientation', 'paper', 'width', 'height'}
PAPER_MUNCHER_VALID_MODES = {'render', 'print'}
EXECUTABLE_NAME: str = 'paper-muncher'
FAKE_REPORT_URL: str = 'http://127.0.0.1:0000/report.html'  # can be anything
FAKE_OUTPUT_URI: str = 'http://stdout'
MIN_PM_VERSION: tuple = (0, 1, 2)


class PaperMuncherProcess:
    """
    class to communicate with paper-muncher.
    this class is made to be used by the PaperMuncher.

    Note: assertion of inputs are not made here
    """
    __slots__ = ('env','mode', 'command', 'html')
    stdin : int = subprocess.PIPE
    stdout : int = subprocess.PIPE
    stderr : int = subprocess.PIPE
    env : Any
    mode : str
    command : list[str]
    html: str

    def __init__(self, env, mode, extra_args=None):
        self.env = env
        self.mode = mode

        base_args = [mode, FAKE_REPORT_URL, '-o' , FAKE_OUTPUT_URI]
        base_args.extend(extra_args or [])
        self.command = base_args

    def __call__(self, html, extra_args=None):
        """This method is aimed to be the autopilot of paper muncher
        and also its base usage
        """
        self.env.set_html(html)

        binary_path = self.locate_executable()

        logger.info("Paper Muncher Executable Path: %s", binary_path)

        if logger.isEnabledFor(logging.DEBUG): 
            logger.debug("Paper Muncher Executable Command: %s", self.command)
            logger.debug("Paper Muncher Executable Args: %s", extra_args)
            logger.debug("Status of Paper Muncher: %s", self.get_executable_status())

        if extra_args:
            self.__override_command(extra_args)

        with subprocess.Popen(
            [binary_path, '--sandbox'] + self.command,
            stdin=self.stdin,
            stdout=self.stdout,
            stderr=self.stderr,
        ) as process:
            try:
                while (request := self.read(process.stdout)) is not None:
                    if request.method == "GET":
                        self.send(
                            process.stdin, self.env.get_asset(request.path))
                    elif request.method == "PUT":
                        self.send(process.stdin, code=200)
                        file_content = process.stdout.read()
                        return file_content
                    else:
                        raise PaperMuncherException(
                            f"Unexpected request method: {request.method}"
                        )
            except Exception as e:
                logger.error("Error during Paper Muncher execution: %s", e)
                raise PaperMuncherException("Error during Paper Muncher execution.") from e
            finally:
                process.terminate()

    def send(self, writter, content=None, code=None):
        if not code:
            if content is None:
                code = 404
            else:
                code = 200

        response = Response(code=code, body=content)
        writter.write(bytes(response))
        writter.flush()

    def read(self, reader):
        return Request(reader=reader)

    def locate_executable(self):
        executable_name = EXECUTABLE_NAME
        if os.name == 'nt':  # Windows
            executable_name += ".exe"
        path = shutil.which(executable_name)
        if path:
            return path
        else:
            raise FileNotFoundError(f"Executable '{executable_name}' not found in PATH.")

    def get_executable_status(self):
        try:
            path = self.locate_executable()
        except FileNotFoundError:
            return 'Not Installed'

        try:
            process = subprocess.Popen(
                [path, '--version'], stdout=subprocess.PIPE, stderr=subprocess.PIPE
            )
        except OSError:
            return 'Dead'

        out, _ = process.communicate()
        version_bare = out.decode('utf-8').strip()
        version_match = re.search(r'(\d+\.\d+\.\d+)', version_bare)

        if not version_match:
            return 'Unknown Version'

        version = tuple(map(int, version_match.group(1).split('.')))

        if version < MIN_PM_VERSION:
            return 'Depreciated'

        return 'Ready'

    def __override_command(self, extra_args):
        for i in range(len(self.command)/2):
            if self.command[i*2] in self.command:
                self.command[i*2+1] = extra_args[i*2+1]


class PaperMuncher:
    """Interface to paper muncher

    This class provides a safe and convenient interface to Paper Muncher.
    It handles pre-checks, argument type filtering, and protects against
    unexpected or silent errors at runtime.

    Note:
        checks assumes that args have a subclass that is backward compatible
        with it's parents  
    """

    __slots__ = ('env', 'mode', 'options',)
    env: Environ
    mode: str
    options: Dict[str, str]

    def __init__(
        self,
        env: Environ,
        mode: str = 'print',
        options: Optional[Dict[str, str]] = None
    ) -> None:
        """Initialize a PaperMuncher instance.

        Args:
            env (Environment): object to get assets.
                Expected: callable get_asset
            mode (str, optional): Either 'print' (for PDFs)
                                      or 'render' (for images).
                                  Defaults to 'print'.
            options (Dict[str, str], optional): Additional rendering options.
                                                Defaults to None.

        Raises:
            TypeError: If env is not a subclass of Environment or doesn't have a get_asset method.
            ValueError: If mode is not 'print' or 'render'.
            TypeError: If options is not a dictionary.
        """

        # assertion block
        if not hasattr(env, 'get_asset'):
            raise TypeError('env should be a subclass of Environment or have a get_asset method!')
        if mode not in PAPER_MUNCHER_VALID_MODES:
            raise ValueError('The mode should be either "render" or "print"')
        if options is not None and not isinstance(options, dict):
            raise TypeError('options should be a dictionary of string: string or None!')

        # setup
        self.env = env
        self.mode = mode
        self.options = options or {}

    def __call__(
        self,
        html_items: Union[str, Iterable[str]],
        mode: Optional[str] = None,
        options: Optional[Dict[str, str]] = None
    ) -> List[bytes]:
        """Generate PDFs or images from HTML, URLs, or file paths.

        Args:
            html_items (Union[str, Iterable[str]]): HTML content, file path, 
            mode (Optional[str]): Either 'print' (PDF) or 'render' (image).
            options (Optional[Dict[str, str]]): Additional settings.

        Returns:
            List[bytes]: Rendered PDFs/images.
        """

        def resolve_input(item: str) -> str:
            """Converts URLs or file paths to HTML string content."""
            parsed = urllib.parse.urlparse(item)
            if parsed.scheme == 'file':
                file_path = parsed.path
            elif os.path.isfile(item):  # assume it's a raw file path
                file_path = item
            else:
                # Assume it's direct HTML
                return item

            try:
                with open(file_path, 'r', encoding='utf-8') as f:
                    return f.read()
            except Exception as e:
                raise RuntimeError(f"Failed to read file {file_path}: {e}")

        # normalize single string to list
        if isinstance(html_items, str):
            html_items = [html_items]

        # validate input
        if not (isinstance(html_items, Iterable) and all(isinstance(x, str) for x in html_items)):
            raise TypeError('html_items should be an iterable of strings!')
        if mode is not None and mode not in PAPER_MUNCHER_VALID_MODES:
            raise ValueError('The mode should be either None, "render" or "print"!')
        if options is not None and not isinstance(options, dict):
            raise TypeError('options should be a dictionary of string: string or None!')

        # fetch/resolve all inputs
        resolved_html = [resolve_input(item) for item in html_items]

        # render
        paper_muncher = PaperMuncherProcess(
            env=self.env,
            mode=mode or self.mode,
            extra_args=self._dict_to_cli_args(options or self.options)
        )

        return [paper_muncher(html) for html in resolved_html]


    @staticmethod
    def _dict_to_cli_args(options: Dict[str, str]) -> List[str]:
        """Helper to convert a dictionary of options into a flat list of
        CLI arguments for Paper Muncher.

        Args:
            options (Dict[str, str]): The CLI additional options for
                                      Paper Muncher.

        Returns:
            List[str]: A flattened list of CLI arguments
                      (e.g., `["--key", "value", ...]`).
        """

        extra_args = []
        unknown_kwargs = []
        for kwarg, value in options.items():
            if not isinstance(value, str):
                raise TypeError(f"Option value for '{kwarg}' must be a string.")
            elif kwarg in PAPER_MUNCHER_ALLOWED_KWARGS:
                extra_args.extend([f"--{kwarg}", value])
            else:
                unknown_kwargs.append(kwarg)

        if unknown_kwargs:
            logger.warning("Unknown kwargs ignored: %s", ", ".join(unknown_kwargs))

        return extra_args

    # Developer-friendly macros
    to_pdfs = partialmethod(__call__, mode='print')
    to_images = partialmethod(__call__, mode='render')

    def to_pdf(self, html: str, options: Optional[Dict[str, str]] = None) -> bytes:
        return self.to_pdfs(html, options=options)[0]

    def to_image(self, html: str, options: Optional[Dict[str, str]] = None) -> bytes:
        return self.to_images(html, options=options)[0]
