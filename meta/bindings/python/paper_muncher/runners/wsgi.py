"""The :mod:`paper_muncher.runners.wsgi` module
provides utilities to simulate HTTP requests to a WSGI application.
It includes functions to generate WSGI environments and simulate
HTTP responses from a WSGI app.
"""
import logging
import os
from collections.abc import Generator
from datetime import datetime, timezone
from email.utils import format_datetime
from typing import Optional
try:
    from wsgiref.types import WSGIEnvironment, WSGIApplication
except ImportError:
    from typing import Any, Callable, Iterable, Tuple
    WSGIStartResponse = Callable[
        [str, list[Tuple[str, str]], Optional[Exception]],
        None
    ]
    WSGIEnvironment = dict[str, Any]
    WSGIApplication = Callable[
        [WSGIEnvironment, WSGIStartResponse],
        Iterable[bytes]
    ]

from werkzeug.test import create_environ, run_wsgi_app

_logger = logging.getLogger(__name__)
SERVER_SOFTWARE = 'Paper Muncher (WSGI Request SIMULATION)'


def generate_environ(
    path: str,
    current_environ: WSGIEnvironment,
) -> WSGIEnvironment:
    """Generate a WSGI environment for the given path.
    This is used to simulate an HTTP request to a WSGI application.
    :param str path: The HTTP request path.
    :return: The WSGI environment dictionary. (See PEP 3333)
    :rtype: WSGIEnvironment
    """
    url, _, query_string = path.partition('?')
    environ = create_environ(
        method='GET',
        path=url,
        query_string=query_string,
        headers={
            'Host': current_environ['HTTP_HOST'],
            'User-Agent': SERVER_SOFTWARE,
            'http_cookie': current_environ['HTTP_COOKIE'],
            'remote_addr': current_environ['REMOTE_ADDR'],
        }
    )
    return environ


def generate_http_response(
    request_path: str,
    application: WSGIApplication,
    environ: WSGIEnvironment,
) -> Generator[bytes, None, None]:
    """Simulate an internal HTTP GET request to an WSGI app and yield
    the HTTP response headers and body as bytes.
    The use of it is mainly permitting to call a wsgi application from an
    inline external application, such as a subprocess requesting resources.

    Note: This function doesn't preserves the thread-local data.

    usage example:
    .. code-block:: python

        from paper_muncher.runners.wsgi import generate_http_response

        for chunk in generate_http_response('/my/request/path'):
            print(chunk.decode())

    :param str request_path: Path to query within the wsgi app.
    :param WSGIApplication application: The WSGI application to query.
    :param WSGIEnvironment environ: The current WSGI environment.
    :yields: Chunks of the full HTTP response to the simulated request.
    :rtype: Generator[bytes, None, None]
    """

    response_iterable, http_status, http_response_headers = run_wsgi_app(
        application, generate_environ(
            path=request_path,
            current_environ=environ
        )
    )

    if "X-Sendfile" in http_response_headers:
        with open(http_response_headers["X-Sendfile"], 'rb') as file:
            now = datetime.now(timezone.utc)
            http_response_status_line_and_headers = (
                f"HTTP/1.1 {http_status}\r\n"
                f"Date: {format_datetime(now, usegmt=True)}\r\n"
                f"Server: {SERVER_SOFTWARE}\r\n"
                f"Content-Length: {os.path.getsize(http_response_headers['X-Sendfile'])}\r\n"
                f"Content-Type: {http_response_headers['Content-Type']}\r\n"
                "\r\n"
            ).encode()

            yield http_response_status_line_and_headers
            yield from file

    else:
        now = datetime.now(timezone.utc)
        http_response_status_line_and_headers = (
            f"HTTP/1.1 {http_status}\r\n"
            f"Date: {format_datetime(now, usegmt=True)}\r\n"
            f"Server: {SERVER_SOFTWARE}\r\n"
            f"Content-Length: {http_response_headers['Content-Length']}\r\n"
            f"Content-Type: {http_response_headers['Content-Type']}\r\n"
            "\r\n"
        ).encode()

        yield http_response_status_line_and_headers
        yield from response_iterable


def wsgi_runner_factory(
    application: WSGIApplication,
    environ: WSGIEnvironment,
):
    """Create a runner function that can be used to generate HTTP responses
    from a WSGI application.

    :param WSGIApplication application: The WSGI application to query.
    :param WSGIEnvironment environ: The current WSGI environment.
        (See PEP 3333) This environment only needs to provide the
        necessary keys to build a new environment for each request.
        (Host, http_cookie, remote_addr)
    :return: A function that takes a request path and yields the HTTP response.
    :rtype: Callable[[str], Generator[bytes, None, None]]
    """
    _logger.debug(
        "Creating WSGI runner for application %r with environ %r",
        application,
        {k: environ[k] for k in (
            'HTTP_HOST',
            'REMOTE_ADDR',
        ) if k in environ}
    )

    def runner(request_path: str) -> Generator[bytes, None, None]:
        return generate_http_response(
            request_path,
            application,
            environ,
        )
    return runner
