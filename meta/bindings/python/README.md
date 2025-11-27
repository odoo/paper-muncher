# Paper Muncher Python Bindings

## Usage examples

### Functional Usage

Paper Muncher includes both synchronous and asynchronous functional APIs.

```python
from paper_muncher.synchronous import render


html = """
<h1>Hello, Paper Muncher!</h1>
<p>This is a simple example of using Paper Muncher in a synchronous context.</p>
"""


def main():
    pdf_bytes = render(html, mode="print")
    with open("output.pdf", "wb") as f:
        f.write(pdf_bytes)
```

**N.B.** The synchronous API is based on a per-OS integration for IO timeouts.

1. For POSIX systems, it relies on selectors.
2. For Windows with Python 3.12+, it puts the file in non-blocking mode.
3. For Windows with Python < 3.12, it falls back to a potentially blocking read without timeout.

```python
from paper_muncher.asynchronous import render


html = """
<h1>Hello, Paper Muncher!</h1>
<p>This is a simple example of using Paper Muncher in an asynchronous context.</p>
"""


async def main():
    pdf_bytes = await render(html, mode="print")
    with open("output_async.pdf", "wb") as f:
        f.write(pdf_bytes)
```

In addition to that it also includes a context based approach to automatically
handle synchronous and asynchronous code execution.

```python
from paper_muncher import render


html = """
<h1>Hello, Paper Muncher!</h1>
<p>This is a simple example of using Paper Muncher in an auto context.</p>
"""

async def main_async():
    pdf_bytes = await render(html, mode="print")
    with open("output_async.pdf", "wb") as f:
        f.write(pdf_bytes)

    print("PDF generated and saved as output_async.pdf")


def main_sync():
    pdf_bytes = render(html, mode="print")
    with open("output_sync.pdf", "wb") as f:
        f.write(pdf_bytes)

    print("PDF generated and saved as output_sync.pdf")
```

### Context Manager Usage

Paper Muncher includes both synchronous and asynchronous context manager APIs.

```python
from paper_muncher.synchronous import rendered


html = """
<h1>Hello, Paper Muncher!</h1>
<p>This is a simple example of using Paper Muncher in a synchronous context.</p>
"""


def main():
    with rendered(html, mode="print") as (pdf_io_stream, std_err):
        pdf = pdf_io_stream.read()

    with open("output_sync.pdf", "wb") as f:
```

**N.B.** The synchronous API is based on a per-OS integration for IO timeouts.

1. For POSIX systems, it relies on selectors.
2. For Windows with Python 3.12+, it puts the file in non-blocking mode.
3. For Windows with Python < 3.12, it falls back to a potentially blocking read without timeout.

```python
from paper_muncher.asynchronous import rendered


html = """
<h1>Hello, Paper Muncher!</h1>
<p>This is a simple example of using Paper Muncher in an asynchronous context.</p>
"""


async def main():
    async with rendered(html, mode="print") as (pdf_stream_reader, std_err):
        pdf = await pdf_stream_reader.read()

    with open("output_async.pdf", "wb") as f:
        f.write(pdf)
```

In addition to that it also includes a context based approach to automatically
handle synchronous and asynchronous code execution.

```python
from paper_muncher import rendered


html = """
<h1>Hello, Paper Muncher!</h1>
<p>This is a simple example of using Paper Muncher in an auto context.</p>
"""

def main_sync():
    with rendered(html, mode="print") as (pdf_io_stream, std_err):
        pdf = pdf_io_stream.read()

    with open("output_sync.pdf", "wb") as f:
        f.write(pdf)

    print("PDF generated and saved as output_sync.pdf")


async def main_async():
    async with rendered(html, mode="print") as (pdf_stream_reader, std_err):
        pdf = await pdf_stream_reader.read()

    with open("output_async.pdf", "wb") as f:
        f.write(pdf)

    print("PDF generated and saved as output_async.pdf")
```

Paper Muncher comes with pre-made integration with some
of the most popular frameworks as well!

* Flask
* Quart
* Fast API
* Django

Your favorite framework is not in the list?
No worries! Some general implementation are also
present!

* agnostic WSGI integration
* agnostic ASGI integration

### Flask

```python
from paper_muncher.frameworks.flask import register_paper_muncher
from flask import Flask, Response

app = Flask(__name__)
register_paper_muncher(app)


@app.route("/")
def index():
    html_content = "<h1>Hello, Paper Muncher with Flask!</h1>"
    pdf_bytes = app.run_paper_muncher(html_content, mode="print")
    return Response(pdf_bytes, mimetype="application/pdf")
```

### Quart

```python
from paper_muncher.frameworks.quart import register_paper_muncher
from quart import Quart, Response

app = Quart(__name__)
register_paper_muncher(app)


@app.route("/")
async def index():
    html_content = "<h1>Hello, Paper Muncher with Quart!</h1>"
    pdf_bytes = await app.run_paper_muncher(html_content, mode="print")
    return Response(pdf_bytes, mimetype="application/pdf")
```

### FastAPI

```python
from fastapi import FastAPI, Response
from paper_muncher.frameworks.fastapi import register_paper_muncher

app = FastAPI()
register_paper_muncher(app)


@app.get("/")
async def index():
    html_content = "<h1>Hello, Paper Muncher with FastAPI!</h1>"
    pdf_bytes = await app.run_paper_muncher(html_content)
    return Response(content=pdf_bytes, media_type="application/pdf")
```

### Django

WSGI.

```python
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
```

ASGI.

```python
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
```
