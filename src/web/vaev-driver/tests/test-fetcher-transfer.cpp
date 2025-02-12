#include <karm-test/macros.h>
#include <vaev-driver/fetcher.h>

namespace Vaev::Driver::Tests {

test$("test-httpipe-fetch-ok") {
    Io::BufferWriter bw;
    Io::TextEncoder mockedStdout{bw};

    auto fetchResp =
        "HTTP/1.1 200 OK\r\n"
        "Server: Odoo\r\n"
        "Content-Length: 4\r\n"
        "\r\n"
        "body"s;
    Io::BufReader mockedInput{bytes(fetchResp)};

    HttpPipe fetcher{
        mockedInput,
        mockedStdout,
    };

    auto url = "file:"_url;
    try$(fetcher.fetch(url));

    expectEq$(bw.bytes(), bytes("GET / HTTP/1.1\r\n\r\n"s));

    return Ok();
}

test$("test-httpipe-fetch-short-body-len") {
    Io::Sink sink;
    Io::TextEncoder mockedStdout{sink};

    auto fetchResp =
        "HTTP/1.1 200 OK\r\n"
        "Server: Odoo\r\n"
        "Content-Length: 5\r\n"
        "\r\n"
        "body"s;
    Io::BufReader mockedInput{bytes(fetchResp)};

    HttpPipe fetcher{
        mockedInput,
        mockedStdout,
    };

    auto url = "file:"_url;
    auto error = fetcher.fetch(url).none();

    expectEq$(error.code(), Error::invalidInput().code());

    return Ok();
}

test$("test-httpipe-fetch-no-body") {
    Io::Sink sink;
    Io::TextEncoder mockedStdout{sink};

    auto fetchResp =
        "HTTP/1.1 200 OK\r\n"
        "Server: Odoo\r\n"
        "\r\n"s;

    Io::BufReader mockedInput{bytes(fetchResp)};

    HttpPipe fetcher{
        mockedInput,
        mockedStdout,
    };

    auto url = "file:"_url;
    auto error = fetcher.fetch(url).none();

    expectEq$(error.code(), Error::invalidData().code());

    return Ok();
}

test$("test-httpipe-fetch-not-ok-code") {
    Io::Sink sink;
    Io::TextEncoder mockedStdout{sink};

    auto fetchResp =
        "HTTP/1.1 404 NOT OK\r\n"
        "Server: Odoo\r\n"
        "\r\n"s;

    Io::BufReader mockedInput{bytes(fetchResp)};

    HttpPipe fetcher{
        mockedInput,
        mockedStdout,
    };

    auto url = "file:"_url;
    auto error = fetcher.fetch(url).none();

    expectEq$(error.code(), Error::invalidData().code());

    return Ok();
}

test$("test-httpipe-fetch-wrong-scheme") {
    Io::Sink sink;
    Io::TextEncoder mockedStdout{sink};

    auto fetchResp =
        "HTTP/1.1 200 OK\r\n"
        "Server: Odoo\r\n"
        "Content-Length: 4\r\n"
        "\r\n"
        "body"s;
    Io::BufReader mockedInput{bytes(fetchResp)};

    HttpPipe fetcher{
        mockedInput,
        mockedStdout,
    };

    auto url = "bundle:"_url;
    auto error = fetcher.fetch(url).none();

    expectEq$(error.code(), Error::invalidInput().code());

    return Ok();
}

test$("test-httpipe-transfer-ok") {
    Io::BufferWriter bw;
    Io::TextEncoder mockedStdout{bw};

    Io::Zero mockedInput;

    HttpPipe fetcher{
        mockedInput,
        mockedStdout,
    };

    auto url = "fancy/pants/path"_url;
    auto transfer = try$(fetcher.transfer(url));

    usize written = 0;
    written += try$(transfer->write(bytes("Ofc: Hello, world!\n"s)));
    written += try$(transfer->write(bytes("and some weird stuff\n"s)));
    written += try$(transfer->write(Buf<Byte>({0, 2, 1})));
    written += try$(transfer->done());

    Io::SScan scan{bw.bytes().cast<char>()};
    auto request = try$(Karm::Net::Http::Request::parse(scan));

    expectEq$(request.method, Net::Http::Method::POST);
    expectEq$(request.header.get("Transfer-Encoding"s), "chunked"s);

    url.path.rooted = false;
    expectEq$(request.path, url.path);

    expectEq$(written, bw.bytes().len());

    auto expectedResponse =
        "POST /fancy/pants/path HTTP/1.1\r\n"
        "Transfer-Encoding: chunked\r\n"
        "\r\n"
        "43\r\n"
        "Ofc: Hello, world!\n"
        "and some weird stuff\n"
        "\x0\x2\x1"
        "\r\n"
        "0\r\n"
        "\r\n"s;

    expectEq$(expectedResponse, Str{bw.bytes().cast<char>()});

    return Ok();
}

test$("test-transfer-after-done") {
    Io::BufferWriter bw;
    Io::TextEncoder mockedStdout{bw};

    Io::Zero mockedInput;

    HttpPipe fetcher{
        mockedInput,
        mockedStdout,
    };

    auto url = ""_url;
    auto transfer = try$(fetcher.transfer(url));

    try$(transfer->done());
    auto error = transfer->write(bytes("oh no!"s)).none();

    expectEq$(error.code(), Error::brokenPipe().code());

    return Ok();
}

} // namespace Vaev::Driver::Tests
