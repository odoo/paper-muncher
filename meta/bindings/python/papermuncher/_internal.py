"""Module with internal functions for handling streams and URLs."""

import asyncio
import logging
import os.path

from io import BytesIO
from functools import wraps
from contextlib import asynccontextmanager
from urllib.parse import urlparse

from typing import Iterable, Optional, overload, Union
from .types import Environment, Streamable

_logger = logging.getLogger(__name__)


@overload
def to_stream(
    instance : Union[str, bytes, BytesIO],
    environment : Optional[Environment] = None,
) -> Iterable[BytesIO]:
    ...

@overload
def to_stream(
    instance : Streamable,
    environment : Optional[Environment] = None,
) -> Iterable[Streamable]:
    ...

@asynccontextmanager
async def to_stream(
    instance : Union[Streamable, str, bytes],
    environment : Optional[Environment] = None,
) -> Iterable[Streamable]:
    """
    Convert various types of input (str, bytes, Streamable) to a stream.
    This function handles different types of input and returns a streamable
    object. It can also handle URLs and file paths, converting them to
    appropriate streamable objects.
    Args:
        instance (Union[Streamable, str, bytes]): The input to convert to a
            stream.
        environment (Optional[Environment]): An optional environment object
            for handling URLs (this arg is also used to know if PM is piped).
    Yields:
        Iterable[Streamable]: A streamable object.
    """
    def has_all_attrs(obj, attrs):
        return all(hasattr(obj, attr) for attr in attrs)

    try:
        if has_all_attrs(instance, ['read', 'readline']):
            yield instance
        else:
            if isinstance(instance, bytes):
                future_stream = instance
            elif isinstance(instance, str):
                if environment is not None and os.path.isfile(instance):
                    with open(instance, 'rb') as file_stream:
                        yield file_stream
                future_stream = instance.encode('utf-8')
            elif hasattr('__bytes__', instance):
                future_stream = bytes(instance)
            elif hasattr('__str__', instance):
                future_stream = str(instance).encode('utf-8')
            else:
                raise TypeError(f"Unsupported type: {type(instance)}")

            url = urlparse(future_stream)
            if environment is not None and url.scheme and url.netloc:
                yield await environment.get_asset(future_stream)
            else:
                stream = BytesIO(future_stream)
                stream.seek(0)
                yield stream

    except Exception as e:
        _logger.error("Error converting to stream: %s", e)
        raise
    else:
        if 'stream' in locals():
            stream.close()


def with_pmoptions_init(cls):
    """Decorator to override __init__ to support PMOptions passthrough."""
    original_init = cls.__init__

    @wraps(original_init)
    def __init__(self, **kwargs):
        # If any value is a PMOptions, use it to populate fields
        option_like = next((v for v in kwargs.values() if isinstance(v, cls)), None)
        if option_like:
            original_init(self, **option_like.__dict__)
        else:
            original_init(self, **kwargs)

    cls.__init__ = __init__
    return cls


class SyncStreamWrapper(Streamable):
    """
    A synchronous wrapper for an asynchronous stream.
    This class allows synchronous code to interact with an
    asynchronous stream by providing synchronous methods for reading
    and iterating over the stream.
    """

    def __init__(self, async_stream: asyncio.StreamReader):
        self.async_stream = async_stream

    def read(self, size=-1) -> bytes:
        return asyncio.run(self._read(size))

    async def _read(self, size=-1) -> bytes:
        return await self.async_stream.read(size)

    def readline(self) -> bytes:
        return asyncio.run(self._readline())

    async def _readline(self) -> bytes:
        return await self.async_stream.readline()

    def __iter__(self):
        return self

    def __next__(self):
        chunk = asyncio.run(self.async_stream.read(4096))
        if not chunk:
            raise StopIteration
        return chunk
