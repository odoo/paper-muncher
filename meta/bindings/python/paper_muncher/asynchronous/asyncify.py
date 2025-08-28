from ..typing import AsyncRunner, Runner

def asyncify_runner(runner: Runner) -> AsyncRunner:
    """Convert a synchronous runner function to an asynchronous one.

    :param Runner runner: A synchronous function that takes a path as input
        and returns bytes.
    :return: An asynchronous version of the input runner function.
    :rtype: AsyncRunner
    """
    async def async_runner(path: str) -> bytes:
        generator = runner(path)
        return generator
    return async_runner
