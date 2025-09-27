from ..asynchronous import rendered as async_rendered
from ..synchronous import rendered as sync_rendered



class RenderedProxy:
    def __init__(self, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs

    def __enter__(self):
        self._sync_cm = sync_rendered(*self.args, **self.kwargs)
        return self._sync_cm.__enter__()

    def __exit__(self, exc_type, exc_val, exc_tb):
        return self._sync_cm.__exit__(exc_type, exc_val, exc_tb)

    async def __aenter__(self):
        self._async_cm = async_rendered(*self.args, **self.kwargs)
        return await self._async_cm.__aenter__()

    async def __aexit__(self, exc_type, exc_val, exc_tb):
        return await self._async_cm.__aexit__(exc_type, exc_val, exc_tb)
