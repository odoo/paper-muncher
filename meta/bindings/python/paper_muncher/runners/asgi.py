import logging
from datetime import datetime, timezone
from email.utils import format_datetime
from typing import AsyncGenerator

import httpx

_logger = logging.getLogger(__name__)
SERVER_SOFTWARE = 'Paper Muncher (ASGI Request SIMULATION)'


async def generate_http_response(
    request_path: str,
    application,
    scope: dict,
) -> AsyncGenerator[bytes, None]:
    """Simulate an internal HTTP GET request to an ASGI app and yield
    the full HTTP response (headers + body) as bytes.

    :param request_path: Path to query within the ASGI app.
    :param application: The ASGI application to query.
    :param scope: The ASGI scope from the current request.
    :yield: Chunks of the full HTTP response.
    """
    headers = {
        "host": scope.get("headers", {}).get(b"host", b"localhost").decode("latin1"),
        "user-agent": SERVER_SOFTWARE,
    }

    client_addr = scope.get("client", ("127.0.0.1", 0))[0]
    if client_addr:
        headers["x-forwarded-for"] = client_addr

    host = headers.get(b"host", b"localhost").decode()

    async with httpx.AsyncClient(
        app=application,
        base_url=host,
    ) as client:
        response = await client.get(request_path, headers=headers)

    now = datetime.now(timezone.utc)
    response_header = (
        f"HTTP/1.1 {response.status_code} {response.reason_phrase}\r\n"
        f"Date: {format_datetime(now, usegmt=True)}\r\n"
        f"Server: {SERVER_SOFTWARE}\r\n"
        f"Content-Length: {len(response.content)}\r\n"
        f"Content-Type: {response.headers.get('Content-Type', 'application/octet-stream')}\r\n"
        "\r\n"
    ).encode()

    yield response_header
    yield response.content


def asgi_runner_factory(application, scope: dict):
    """Create a runner coroutine that can generate HTTP responses
    from an ASGI application using the current request scope.

    :param application: The ASGI app.
    :param scope: The current ASGI request scope.
    :return: Async function taking a request path and yielding bytes.
    """
    _logger.debug(
        "Creating ASGI runner for application %r with scope %r",
        application,
        {
            "client": scope.get("client"),
            "headers": dict(scope.get("headers", [])),
        }
    )

    async def runner(request_path: str) -> AsyncGenerator[bytes, None]:
        async for chunk in generate_http_response(request_path, application, scope):
            yield chunk

    return runner
