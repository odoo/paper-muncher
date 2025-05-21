from .path import REPORT_URI
from ..types import Environment
from ..exceptions import PaperMuncherException
from ..communication import BindingResponse
from .._internal import to_stream


class DefaultEnvironment(Environment):

    def __init__(self, from_doc=None) -> None:
        self.doc = from_doc

    async def get_asset(self, path: str) -> None:
        if path in REPORT_URI:
            return self.doc

    async def handle_get(self, stdin, path):
        asset = to_stream(await self.get_asset(path))
        response = BindingResponse(code=200 if asset else 404, body=asset)
        return response

    async def handle_put(self, stdin) -> None:
        return BindingResponse(code=200)

    async def handle_request(self, stdin, request) -> BindingResponse:
        if request.method == 'GET':
            await self.handle_get(stdin, request.path)
        elif request.method == 'PUT':
            await self.handle_put(stdin)
        else:
            raise PaperMuncherException(
                f"Unsupported method: {request.method}"
            )
