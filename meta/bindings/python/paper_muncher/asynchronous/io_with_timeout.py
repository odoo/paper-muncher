from asyncio import wait_for, TimeoutError as AsyncTimeoutError

# typing imports
from asyncio import StreamReader, StreamWriter


async def readline_with_timeout(
    reader: StreamReader,
    timeout: int,
) -> bytes:
    """Read a full line ending with '\\n' from an asyncio StreamReader within a
    timeout.

    :param asyncio.StreamReader reader: StreamReader to read from
        (must be in binary mode).
    :param int timeout: Max seconds to wait for line data.
    :return: A line of bytes ending in '\\n'.
    :rtype: bytes
    :raises TimeoutError: If timeout is reached before a line is read.
    :raises EOFError: If EOF is reached before a line is read.
    """
    line_buffer = bytearray()

    while True:
        try:
            next_byte = await wait_for(reader.read(1), timeout=timeout)
        except AsyncTimeoutError as ate:
            raise TimeoutError("Timeout reached while reading line") from ate

        if not next_byte:
            raise EOFError("EOF reached while reading line")

        line_buffer += next_byte
        if next_byte == b'\n':
            break

    return bytes(line_buffer)


async def read_all_with_timeout(
    reader: StreamReader,
    timeout: int,
    chunk_size: int,
) -> bytes:
    """Read all data from an asyncio StreamReader until EOF, with a timeout per
    chunk.

    :param asyncio.StreamReader reader: StreamReader to read from.
    :param int timeout: Timeout in seconds for the entire read operation.
    :param int chunk_size: Number of bytes to read per chunk.
    :return: All bytes read until EOF.
    :rtype: bytes
    :raises TimeoutError: If no data is read within the timeout period.
    """
    data = bytearray()
    while True:
        try:
            chunk = await wait_for(reader.read(chunk_size), timeout=timeout)
        except AsyncTimeoutError as ate:
            raise TimeoutError("Timeout reached while reading data") from ate

        if not chunk:
            break
        data.extend(chunk)

    return bytes(data)


async def write_with_timeout(
    writer: StreamWriter,
    data: bytes,
    timeout: int,
) -> None:
    """Write data to an asyncio StreamWriter.

    :param asyncio.StreamWriter writer: StreamWriter to write to.
    :param bytes data: Data to write.
    :param int timeout: Timeout in seconds for the drain operation.
    :return: None
    :rtype: None
    :raises TimeoutError: If the drain operation exceeds the timeout.
    """
    writer.write(data)  # always non-blocking
    try:
        await wait_for(writer.drain(), timeout)
    except AsyncTimeoutError as ate:
        writer.close()
        raise TimeoutError("Timeout reached while writing data") from ate


if __name__ == "__main__":
    import asyncio

    async def main():
        proc = await asyncio.create_subprocess_exec(
            "cat",
            stdin=asyncio.subprocess.PIPE,
            stdout=asyncio.subprocess.PIPE,
        )

        try:
            await write_with_timeout(proc.stdin, b"Hello, World!\n", timeout=5)
            proc.stdin.close()

            output = await readline_with_timeout(proc.stdout, timeout=5)
            print(f"Output: {output.decode().strip()}")

            await proc.wait()
        finally:
            if proc.returncode is None:
                proc.terminate()
                try:
                    await wait_for(proc.wait(), timeout=5)
                except AsyncTimeoutError:
                    proc.kill()
                    await proc.wait()

    asyncio.run(main())
