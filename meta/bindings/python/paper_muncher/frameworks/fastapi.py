"""The :mod:`paper_muncher.frameworks.fastapi` module
provides integration with FastAPI applications.
"""

from contextvars import ContextVar
from ..runners.asgi import asgi_runner_factory
from ..asynchronous import render


_current_scope: ContextVar[dict] = ContextVar("current_scope")

def register_paper_muncher(fastapi_app):
    """
    Registers the `run_paper_muncher` method on a FastAPI application.

    Automatically adds middleware to capture the ASGI scope on each request.
    """

    async def run_paper_muncher(content, mode="print", **options):
        scope = _current_scope.get()
        runner = asgi_runner_factory(fastapi_app, scope)
        return await render(content, mode=mode, runner=runner, **options)

    fastapi_app.run_paper_muncher = run_paper_muncher

    @fastapi_app.middleware("http")
    async def capture_scope_middleware(request, call_next):
        token = _current_scope.set(request.scope)
        try:
            return await call_next(request)
        finally:
            _current_scope.reset(token)
