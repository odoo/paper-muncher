"""The :mod:`paper_muncher.frameworks.quart` module
provides integration with Quart applications.
"""

from quart import request
from ..runners.asgi import asgi_runner_factory
from ..asynchronous import render


def register_paper_muncher(quart_application):
    async def run_paper_muncher(content, mode="print", **options):
        runner = asgi_runner_factory(quart_application, request.scope)
        return await render(content, mode=mode, runner=runner, **options)

    quart_application.run_paper_muncher = run_paper_muncher
