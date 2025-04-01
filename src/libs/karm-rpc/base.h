#pragma once

#include <karm-async/promise.h>
#include <karm-async/queue.h>
#include <karm-base/map.h>
#include <karm-base/tuple.h>
#include <karm-io/pack.h>
#include <karm-logger/logger.h>

#include "hooks.h"

namespace marK::Rpc {

// MARK: Primitive Types -------------------------------------------------------

struct Port : public Distinct<u64, struct _PortTag> {
    static Port const INVALID;
    static Port const BUS;
    static Port const BROADCAST;

    using Distinct::Distinct;

    void repr(Io::Emit& e) const {
        if (*this == INVALID)
            e("invalid");
        else if (*this == BUS)
            e("bus");
        else if (*this == BROADCAST)
            e("broadcast");
        else
            e("{}", value());
    }
};

constexpr Port Port::INVALID{0};
constexpr Port Port::BUS{Limits<u64>::MAX};
constexpr Port Port::BROADCAST{Limits<u64>::MAX - 1};

struct Header {
    u64 seq;
    Port from;
    Port to;
    Meta::Id mid;

    void repr(Io::Emit& e) const {
        e("(header seq: {}, from: {}, to: {}, mid: {:016x})", seq, from, to, mid);
    }
};

static_assert(Meta::TrivialyCopyable<Header>);

struct Message {
    static constexpr usize CAP = 4096;

    union {
        struct {
            Header _header;
            Array<u8, CAP - sizeof(Header)> _payload;
        };

        Array<u8, CAP> _buf;
    };

    usize _len = 0;

    Array<Sys::Handle, 16> _hnds;
    usize _hndsLen = 0;

    Header& header() {
        return _header;
    }

    Header const& header() const {
        return _header;
    }

    usize len() const {
        return _len;
    }

    Bytes bytes() {
        return sub(_buf, 0, len());
    }

    Slice<Sys::Handle> handles() {
        return sub(_hnds, 0, _hndsLen);
    }

    template <typename T>
    bool is() const {
        return _header.mid == Meta::idOf<T>();
    }

    template <typename T, typename... Args>
    static Res<Message> packReq(Port to, u64 seq, Args&&... args) {
        T payload{std::forward<Args>(args)...};

        Message msg;
        msg._header = {
            seq,
            Port::INVALID,
            to,
            Meta::idOf<T>(),
        };
        Io::BufWriter reqBuf{msg._payload};
        Io::PackEmit reqPack{reqBuf};

        try$(Io::pack(reqPack, payload));

        msg._len = try$(Io::tell(reqBuf)) + sizeof(Header);

        return Ok(std::move(msg));
    }

    template <typename T, typename... Args>
    Res<Message> packResp(Args&&... args) {
        typename T::Response payload{std::forward<Args>(args)...};

        Message resp;
        resp._header = {
            header().seq,
            header().to,
            header().from,
            Meta::idOf<typename T::Response>(),
        };

        Io::BufWriter respBuf{resp._payload};
        Io::PackEmit respPack{respBuf};

        try$(Io::pack(respPack, payload));

        resp._len = try$(Io::tell(respBuf)) + sizeof(Header);

        return Ok(std::move(resp));
    }

    template <typename T>
    Res<T> unpack() {
        Io::PackScan s{bytes(), handles()};
        if (not is<T>())
            return Error::invalidData("unexpected message");
        try$(Io::unpack<Header>(s));
        return Io::unpack<T>(s);
    }
};

// MARK: Primitive Operations --------------------------------------------------

template <typename T, typename... Args>
Res<> rpcSend(Sys::IpcConnection& con, Port to, u64 seq, Args&&... args) {
    Message msg = Message::packReq<T>(to, seq, std::forward<Args>(args)...).take();

    try$(con.send(msg.bytes(), msg.handles()));
    return Ok();
}

static inline Async::Task<Message> rpcRecvAsync(Sys::IpcConnection& con) {
    Message msg;
    auto [bufLen, hndsLen] = co_trya$(con.recvAsync(msg._buf, msg._hnds));
    if (bufLen < sizeof(Header))
        co_return Error::invalidData("invalid message");
    msg._len = bufLen;
    msg._hndsLen = hndsLen;

    co_return msg;
}

// MARK: Rpc -------------------------------------------------------------------

struct Endpoint : Meta::Pinned {
    Sys::IpcConnection _con;
    Map<u64, Async::_Promise<Message>> _pending{};
    Async::Queue<Message> _incoming{};
    u64 _seq = 1;

    Endpoint(Sys::IpcConnection con);

    static Endpoint create(Sys::Context& ctx);

    static Async::Task<> _receiverTask(Endpoint& self) {
        while (true) {
            Message msg = co_trya$(rpcRecvAsync(self._con));
            auto header = msg._header;

            if (self._pending.has(header.seq)) {
                auto promise = self._pending.take(header.seq);
                promise.resolve(std::move(msg));
            } else {
                self._incoming.enqueue(std::move(msg));
            }
        }
    }

    template <typename T, typename... Args>
    Res<> send(Port port, Args&&... args) {
        return rpcSend<T>(_con, port, _seq++, std::forward<Args>(args)...);
    }

    Async::Task<Message> recvAsync() {
        co_return Ok(co_await _incoming.dequeueAsync());
    }

    template <typename T>
    Res<> resp(Message& msg, Res<typename T::Response> message) {
        auto header = msg._header;
        if (not message)
            return rpcSend<Error>(_con, header.from, header.seq, message.none());
        return rpcSend<typename T::Response>(_con, header.from, header.seq, message.take());
    }

    template <typename T, typename... Args>
    Async::Task<typename T::Response> callAsync(Port port, Args&&... args) {
        auto seq = _seq++;
        Async::_Promise<Message> promise;
        auto future = promise.future();
        _pending.put(seq, std::move(promise));

        co_try$(rpcSend<T>(_con, port, seq, std::forward<Args>(args)...));

        Message msg = co_await future;

        if (msg.is<Error>())
            co_return co_try$(msg.unpack<Error>());

        if (not msg.is<typename T::Response>())
            co_return Error::invalidInput("unexpected response");

        co_return Ok(co_try$(msg.unpack<typename T::Response>()));
    }
};

Endpoint& globalEndpoint();

} // namespace Karm::Rpc
