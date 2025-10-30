import os
import asyncio

from django.conf import settings
from django.core.asgi import get_asgi_application
from django.http import HttpResponse
from django.urls import path
from django.core.management import execute_from_command_line

from asgiref.sync import async_to_sync
from hypercorn.config import Config
from hypercorn.asyncio import serve

from paper_muncher.frameworks.django_asgi import register_paper_muncher  # Your patch


BASE_DIR = os.path.dirname(__file__)
settings.configure(
    DEBUG=True,
    ROOT_URLCONF=__name__,
    SECRET_KEY="dummy",
    ALLOWED_HOSTS=["*"],
    MIDDLEWARE=[],
)


def index(request):
    html = "<h1>Hello from Django!</h1>"
    pdf = async_to_sync(application.run_paper_muncher)(html)
    return HttpResponse(pdf, content_type="application/pdf")


urlpatterns = [
    path("", index),
]

os.environ.setdefault("DJANGO_SETTINGS_MODULE", "__main__")
django_asgi_app = get_asgi_application()
application = register_paper_muncher(django_asgi_app)


if __name__ == "__main__":
    config = Config()
    config.bind = ["127.0.0.1:5000"]
    config.use_reloader = True
    asyncio.run(serve(application, config))
