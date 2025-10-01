from fastapi import FastAPI, Response
from paper_muncher.frameworks.fastapi import register_paper_muncher

app = FastAPI()
register_paper_muncher(app)


@app.get("/")
async def index():
    html_content = "<h1>Hello, Paper Muncher with FastAPI!</h1>"
    pdf_bytes = await app.run_paper_muncher(html_content)
    return Response(content=pdf_bytes, media_type="application/pdf")


if __name__ == "__main__":
    import asyncio
    from hypercorn.asyncio import serve
    from hypercorn.config import Config

    config = Config()
    config.bind = ["127.0.0.1:5000"]
    config.use_reloader = True 
    asyncio.run(serve(app, config))
