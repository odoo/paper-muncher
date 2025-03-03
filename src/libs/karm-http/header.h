#pragma once

#include <karm-io/aton.h>
#include <karm-io/fmt.h>

namespace Karm::Net::Http {

struct Version {
    u8 major;
    u8 minor;

    static Res<Version> parse(Io::SScan& s);

    Res<> unparse(Io::TextWriter& w);

    bool operator==(Version const& other) const = default;

    auto operator<=>(Version const& other) const = default;
};

struct Header : public Map<String, String> {
    using Map<String, String>::Map;

    void add(Str const& key, Str value);

    Res<> parse(Io::SScan& s);

    Res<> unparse(Io::TextWriter& w);
};

} // namespace Karm::Net::Http

template <>
struct Karm::Io::Formatter<Karm::Net::Http::Version> {
    Res<> format(Io::TextWriter& writer, Karm::Net::Http::Version version) {
        return Io::format(writer, "HTTP/{}.{}", version.major, version.minor);
    }
};
