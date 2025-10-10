"""The :mod:`paper_muncher.frameworks.django_wsgi` module
provides integration with Django WSGI applications.
"""

from contextvars import ContextVar
from ..runners.wsgi import wsgi_runner_factory
from ..synchronous import render


_current_environ: ContextVar[dict] = ContextVar("current_environ")

def register_paper_muncher(django_wsgi_app):
    """
    Registers the `run_paper_muncher` method on a Django WSGI app object.
    Adds middleware to capture the WSGI environ.
    """
    class PaperMuncherEnvironMiddleware:
        def __init__(self, app):
            self.app = app

        def __call__(self, environ, start_response):
            token = _current_environ.set(environ)
            try:
                return self.app(environ, start_response)
            finally:
                _current_environ.reset(token)

    def run_paper_muncher(content, mode="print", **options):
        environ = _current_environ.get()
        runner = wsgi_runner_factory(django_wsgi_app, environ)
        return render(content, mode=mode, runner=runner, **options)

    django_wsgi_app.run_paper_muncher = run_paper_muncher

    middleware = PaperMuncherEnvironMiddleware(django_wsgi_app)
    middleware.run_paper_muncher = run_paper_muncher

    return middleware
