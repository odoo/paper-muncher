"""The :mod:`paper_muncher.typing` module provides
type definitions used in Paper Muncher.
"""

from typing import Callable


class Runner(Callable):
    def __call__(self, path: str) -> bytes:
        pass


class AsyncRunner(Callable):
    async def __call__(self, path: str) -> bytes:
        pass
