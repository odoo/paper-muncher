from paper_muncher.frameworks.flask import register_paper_muncher
from flask import Flask, Response

app = Flask(__name__)
register_paper_muncher(app)


@app.route("/")
def index():
    html_content = "<h1>Hello, Paper Muncher with Flask!</h1>"
    pdf_bytes = app.run_paper_muncher(html_content, mode="print")
    return Response(pdf_bytes, mimetype="application/pdf")


if __name__ == "__main__":
    app.run(debug=True)
