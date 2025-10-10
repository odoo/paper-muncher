from asyncio import wait_for, TimeoutError as AsyncTimeoutError
from asyncio.subprocess import create_subprocess_exec
from contextlib import asynccontextmanager


@asynccontextmanager
async def Popen(*args, **kwargs):
    """Async context manager for asyncio subprocess that sets non-blocking I/O
    for stdin, stdout, and stderr.
    This is necessary for Windows to avoid deadlocks when reading
    from subprocess streams.

    :param args: Positional arguments for subprocess.Popen.
    :param kwargs: Keyword arguments for subprocess.Popen.
    :return: A context manager that yields the subprocess.Popen object.
    """
    if isinstance(args[0], list):
        args = args[0] 
    proc = await create_subprocess_exec(*args, **kwargs)
    try:
        yield proc
    finally:
        if proc.returncode is None:
            proc.terminate()
            try:
                await wait_for(proc.wait(), timeout=5)
            except AsyncTimeoutError:
                proc.kill()
                await proc.wait()


if __name__ == "__main__":
    import asyncio

    async def main():
        async with Popen("cat", stdin=asyncio.subprocess.PIPE, stdout=asyncio.subprocess.PIPE) as proc:
            proc.stdin.write(b"Hello, World!\n")
            await proc.stdin.drain()
            proc.stdin.close()
            output = await proc.stdout.read()
            print(f"Output: {output.decode().strip()}")

    asyncio.run(main())
