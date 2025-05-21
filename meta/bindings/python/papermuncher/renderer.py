import asyncio
import logging
from contextlib import contextmanager, asynccontextmanager
from typing import Any, Optional

from .binding import paper_muncher  # your asynccontextmanager

_logger = logging.getLogger(__name__)


@asynccontextmanager
async def render_async(
    xhtml: Any,
    environment: Optional[Any] = None,
    **rendering_options: dict[str, str],
):
    async with paper_muncher(doc=xhtml, environment=environment, **rendering_options) as result:
        yield result

@contextmanager
def render_sync(
    xhtml: Any,
    environment: Optional[Any] = None,
    **rendering_options: dict[str, str],
):
    async def runner():
        async with paper_muncher(doc=xhtml, environment=environment, **rendering_options) as result:
            return result

    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    try:
        result = loop.run_until_complete(runner())
        yield result
    finally:
        loop.close()


def render(xhtml, environment=None, **rendering_options):
    try:
        asyncio.get_running_loop()
    except RuntimeError:
        _logger.info("No running loop, using sync render")
        return render_sync(xhtml, environment, **rendering_options)
    else:
        _logger.info("Running loop detected, using async render")
        return render_async(xhtml, environment, **rendering_options)


def to_pdf(xhtml, environment=None, **rendering_options):
    return render(xhtml, environment, mode="print", **rendering_options)


def to_image(xhtml, environment=None, **rendering_options):
    return render(xhtml, environment, mode="render", **rendering_options)
