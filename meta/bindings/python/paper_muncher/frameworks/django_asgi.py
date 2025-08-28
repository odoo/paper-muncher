"""The :mod:`paper_muncher.frameworks.django_asgi` module
provides integration with Django ASGI applications.
"""

from contextvars import ContextVar
from ..runners.asgi import asgi_runner_factory
from ..asynchronous import render


_current_scope: ContextVar[dict] = ContextVar("current_scope")


def register_paper_muncher(django_asgi_app):
    """
    Registers the `run_paper_muncher` method on a Django ASGI app object.
    Adds middleware to capture the scope.
    """
    class PaperMuncherScopeMiddleware:
        def __init__(self, app):
            self.app = app

        async def __call__(self, scope, receive, send):
            token = _current_scope.set(scope)
            try:
                await self.app(scope, receive, send)
            finally:
                _current_scope.reset(token)

    async def run_paper_muncher(content, mode="print", **options):
        scope = _current_scope.get()
        runner = asgi_runner_factory(django_asgi_app, scope)
        return await render(content, mode=mode, runner=runner, **options)

    django_asgi_app.run_paper_muncher = run_paper_muncher
    middleware = PaperMuncherScopeMiddleware(django_asgi_app)
    middleware.run_paper_muncher = run_paper_muncher

    return middleware
