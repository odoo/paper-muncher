from typing import Protocol, Union
from io import BytesIO
from .communication import BindingResponse

class Streamable(Protocol):
    def read(self, size: int = -1) -> bytes: ...
    def readline(self) -> bytes: ...

class Environment(Protocol):
    async def handle_request(self, stdin, request) -> BindingResponse: ...
