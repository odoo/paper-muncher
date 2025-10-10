from paper_muncher.frameworks.quart import register_paper_muncher
from quart import Quart, Response

app = Quart(__name__)
register_paper_muncher(app)


@app.route("/")
async def index():
    html_content = "<h1>Hello, Paper Muncher with Quart!</h1>"
    pdf_bytes = await app.run_paper_muncher(html_content, mode="print")
    return Response(pdf_bytes, mimetype="application/pdf")


if __name__ == "__main__":
    app.run(debug=True)
