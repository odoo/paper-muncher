#include <seccomp.h>

//
#include <karm-sys/_embed.h>

#include "../utils.h"

namespace Karm::Sys::_Embed {

Res<> hardenSandbox() {
    auto [repo, format] = try$(Posix::repoRoot());

    if (chroot(repo.buf()) < 0)
        return Posix::fromLastErrno();

    if (chdir("/") < 0)
        return Posix::fromLastErrno();

    Posix::overrideRepo({"/"s, format});

    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_KILL_PROCESS);
    if (!ctx)
        return Posix::fromLastErrno();
    Defer cleanupSeccomp = [&] {
        seccomp_release(ctx);
    };

    if (auto it = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0); it < 0)
        return Posix::fromErrno(-it);

    if (auto it = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0); it < 0)
        return Posix::fromErrno(-it);

    if (auto it = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(close), 0); it < 0)
        return Posix::fromErrno(-it);

    if (auto it = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(open), 0); it < 0)
        return Posix::fromErrno(-it);

    if (seccomp_load(ctx) < 0)
        return Posix::fromLastErrno();

    return Ok();
}

} // namespace Karm::Sys::_Embed
