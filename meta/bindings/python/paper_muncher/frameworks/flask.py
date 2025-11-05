"""The :mod:`paper_muncher.frameworks.flask` module
provides integration with Flask applications.
"""

from ..runners.wsgi import wsgi_runner_factory
from ..synchronous import render
from flask import request


def register_paper_muncher(flask_application):
    def run_paper_muncher(content, mode="print", **options):
        runner = wsgi_runner_factory(flask_application, request.environ)
        return render(content, mode=mode, runner=runner, **options)

    flask_application.run_paper_muncher = run_paper_muncher
