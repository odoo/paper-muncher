"""The :mod:`paper_muncher.frameworks.wsgi_app` module
provides integration with generic WSGI applications.
"""

from contextvars import ContextVar
from ..runners.wsgi import wsgi_runner_factory
from ..synchronous import render


_current_environ: ContextVar[dict] = ContextVar("current_environ")

def patch(application):
    """
    Monkey-patches a WSGI application to add `run_paper_muncher()` that
    can render content using the current WSGI environ from the request.

    Requires a WSGI middleware to set the environ per request.
    """

    def run_paper_muncher(content, mode="print", **options):
        environ = _current_environ.get()
        runner = wsgi_runner_factory(application, environ)
        return render(content, mode=mode, runner=runner, **options)

    application.run_paper_muncher = run_paper_muncher

    return _current_environ
