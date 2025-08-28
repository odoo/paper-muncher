"""The :mod:`paper_muncher.frameworks.asgi_app` module
provides integration with generic ASGI applications.
"""

from contextvars import ContextVar
from ..runners.asgi import asgi_runner_factory
from ..asynchronous import render


_current_scope: ContextVar[dict] = ContextVar("current_scope")

def register_paper_muncher(asgi_application):
    async def run_paper_muncher(content, mode="print", **options):
        scope = _current_scope.get()
        runner = asgi_runner_factory(asgi_application, scope)
        return await render(content, mode=mode, runner=runner, **options)

    asgi_application.run_paper_muncher = run_paper_muncher

    return _current_scope
