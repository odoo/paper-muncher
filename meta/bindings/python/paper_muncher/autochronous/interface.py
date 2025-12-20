import asyncio

from .proxy import RenderedProxy
from ..asynchronous import render as async_render
from ..synchronous import render as sync_render

def render(*args, **kwargs):
    try:
        loop = asyncio.get_running_loop()
    except RuntimeError:
        loop = None

    if loop is not None and loop.is_running():
        return async_render(*args, **kwargs)
    return sync_render(*args, **kwargs)

def rendered(*args, **kwargs):
    return RenderedProxy(*args, **kwargs)
