import os
from wsgiref.simple_server import make_server

from django.conf import settings
from django.core.wsgi import get_wsgi_application
from django.http import HttpResponse
from django.urls import path
from django.core.management import execute_from_command_line

from paper_muncher.frameworks.django_wsgi import register_paper_muncher


BASE_DIR = os.path.dirname(__file__)
settings.configure(
    DEBUG=True,
    ROOT_URLCONF=__name__,
    SECRET_KEY="dummy",
    ALLOWED_HOSTS=["*"],
    MIDDLEWARE=[],
)


def index(request):
    html = "<h1>Hello from Django WSGI!</h1>"
    pdf = application.run_paper_muncher(html)
    return HttpResponse(pdf, content_type="application/pdf")


urlpatterns = [
    path("", index),
]

os.environ.setdefault("DJANGO_SETTINGS_MODULE", "__main__")

django_wsgi_app = get_wsgi_application()
application = register_paper_muncher(django_wsgi_app)


if __name__ == "__main__":
    with make_server("127.0.0.1", 5000, application) as httpd:
        print("Serving on http://127.0.0.1:5000")
        httpd.serve_forever()
