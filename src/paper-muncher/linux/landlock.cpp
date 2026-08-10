module;

#include <fcntl.h>
#include <linux/landlock.h>
#include <sys/syscall.h>
#include <unistd.h>

export module PaperMuncher.Linux:landlock;

import Karm.Sys.Posix;

import Karm.Core;
import Karm.Logger;

using namespace Karm;

namespace PaperMuncher {

#ifndef LANDLOCK_ACCESS_FS_REFER
#    define LANDLOCK_ACCESS_FS_REFER 0
#endif

#ifndef LANDLOCK_ACCESS_FS_TRUNCATE
#    define LANDLOCK_ACCESS_FS_TRUNCATE 0
#endif

#ifndef LANDLOCK_ACCESS_NET_BIND_TCP
#    define LANDLOCK_ACCESS_NET_BIND_TCP 0
#endif

#ifndef LANDLOCK_ACCESS_NET_BIND_TCP
#    define LANDLOCK_ACCESS_NET_BIND_TCP 0
#    define LANDLOCK_ACCESS_NET_CONNECT_TCP 0
#endif

#ifndef LANDLOCK_ACCESS_FS_IOCTL_DEV
#    define LANDLOCK_ACCESS_FS_IOCTL_DEV 0
#endif

#ifndef LANDLOCK_SCOPE_ABSTRACT_UNIX_SOCKET
#    define LANDLOCK_SCOPE_ABSTRACT_UNIX_SOCKET 0
#    define LANDLOCK_SCOPE_SIGNAL 0
#endif

#ifndef LANDLOCK_ACCESS_FS_RESOLVE_UNIX
#    define LANDLOCK_ACCESS_FS_RESOLVE_UNIX 0
#endif

#ifndef LANDLOCK_ACCESS_NET_BIND_UDP
#    define LANDLOCK_ACCESS_NET_BIND_UDP 0
#    define LANDLOCK_ACCESS_NET_CONNECT_SEND_UDP 0
#endif

export Res<> landlockAllowReadingDirectory(Vec<Str> dirs) {
    int abi = syscall(__NR_landlock_create_ruleset, NULL, 0, LANDLOCK_CREATE_RULESET_VERSION);

    if (abi < 0) {
        logWarn("landlock is disabled on this kernel (abi:{})", abi);
        return Ok();
    }

    // NOTE: To be compatible with older Linux versions, we detect the available Landlock ABI version, and only use the available subset of access rights.
    // https://docs.kernel.org/userspace-api/landlock.html#:~:text=To%20be%20compatible%20with%20older%20Linux
    landlock_ruleset_attr attr = {
        .handled_access_fs =
            LANDLOCK_ACCESS_FS_EXECUTE |
            LANDLOCK_ACCESS_FS_WRITE_FILE |
            LANDLOCK_ACCESS_FS_READ_FILE |
            LANDLOCK_ACCESS_FS_READ_DIR |
            LANDLOCK_ACCESS_FS_REMOVE_DIR |
            LANDLOCK_ACCESS_FS_REMOVE_FILE |
            LANDLOCK_ACCESS_FS_MAKE_CHAR |
            LANDLOCK_ACCESS_FS_MAKE_DIR |
            LANDLOCK_ACCESS_FS_MAKE_REG |
            LANDLOCK_ACCESS_FS_MAKE_SOCK |
            LANDLOCK_ACCESS_FS_MAKE_FIFO |
            LANDLOCK_ACCESS_FS_MAKE_BLOCK |
            LANDLOCK_ACCESS_FS_MAKE_SYM |
            LANDLOCK_ACCESS_FS_REFER |
            LANDLOCK_ACCESS_FS_TRUNCATE |
            LANDLOCK_ACCESS_FS_IOCTL_DEV |
            LANDLOCK_ACCESS_FS_RESOLVE_UNIX,
        .handled_access_net =
            LANDLOCK_ACCESS_NET_BIND_TCP |
            LANDLOCK_ACCESS_NET_CONNECT_TCP |
            LANDLOCK_ACCESS_NET_BIND_UDP |
            LANDLOCK_ACCESS_NET_CONNECT_SEND_UDP,
        .scoped =
            LANDLOCK_SCOPE_ABSTRACT_UNIX_SOCKET |
            LANDLOCK_SCOPE_SIGNAL,
    };

    switch (abi) {
    case 1:
        /* Removes LANDLOCK_ACCESS_FS_REFER for ABI < 2 */
        attr.handled_access_fs &= ~LANDLOCK_ACCESS_FS_REFER;
        __attribute__((fallthrough));
    case 2:
        /* Removes LANDLOCK_ACCESS_FS_TRUNCATE for ABI < 3 */
        attr.handled_access_fs &= ~LANDLOCK_ACCESS_FS_TRUNCATE;
        __attribute__((fallthrough));
    case 3:
        /* Removes network support for ABI < 4 */
        attr.handled_access_net &=
            ~(LANDLOCK_ACCESS_NET_BIND_TCP |
              LANDLOCK_ACCESS_NET_CONNECT_TCP);
        __attribute__((fallthrough));
    case 4:
        /* Removes LANDLOCK_ACCESS_FS_IOCTL_DEV for ABI < 5 */
        attr.handled_access_fs &= ~LANDLOCK_ACCESS_FS_IOCTL_DEV;
        __attribute__((fallthrough));
    case 5:
        /* Removes LANDLOCK_SCOPE_* for ABI < 6 */
        attr.scoped &= ~(LANDLOCK_SCOPE_ABSTRACT_UNIX_SOCKET | LANDLOCK_SCOPE_SIGNAL);
        __attribute__((fallthrough));
    case 6 ... 8:
        /* Removes LANDLOCK_ACCESS_FS_RESOLVE_UNIX for ABI < 9 */
        attr.handled_access_fs &= ~LANDLOCK_ACCESS_FS_RESOLVE_UNIX;
        __attribute__((fallthrough));
    case 9:
        /* Removes LANDLOCK_ACCESS_NET_*_UDP for ABI < 10 */
        attr.handled_access_net &=
            ~(LANDLOCK_ACCESS_NET_BIND_UDP |
              LANDLOCK_ACCESS_NET_CONNECT_SEND_UDP);
    }

    int rulesetFd = syscall(__NR_landlock_create_ruleset, &attr, sizeof(attr), 0);
    if (rulesetFd < 0)
        return Posix::fromLastErrno();

    Defer closeRuleset{[&] {
        close(rulesetFd);
    }};

    for (auto dir : dirs) {
        int dirFd = open(dir.buf(), O_PATH | O_DIRECTORY | O_CLOEXEC);
        if (dirFd < 0)
            return Posix::fromLastErrno();

        Defer closeDir{[&] {
            close(dirFd);
        }};

        landlock_path_beneath_attr pathAttr = {
            .allowed_access =
                LANDLOCK_ACCESS_FS_READ_FILE |
                LANDLOCK_ACCESS_FS_READ_DIR,
            .parent_fd = dirFd
        };

        if (syscall(__NR_landlock_add_rule, rulesetFd, LANDLOCK_RULE_PATH_BENEATH, &pathAttr, 0) < 0)
            return Posix::fromLastErrno();
    }

    if (syscall(__NR_landlock_restrict_self, rulesetFd, 0) < 0)
        return Posix::fromLastErrno();

    return Ok();
}

} // namespace PaperMuncher
