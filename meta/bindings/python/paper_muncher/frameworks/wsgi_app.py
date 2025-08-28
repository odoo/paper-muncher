from ..runners.wsgi import wsgi_runner_factory
from ..synchronous import render


def patch(application):
    def run_paper_muncher(content, mode="print", **options):
        runner = wsgi_runner_factory(application, application.request.environ)
        return render(content, mode=mode, runner=runner, **options)

    application.run_paper_muncher = run_paper_muncher
