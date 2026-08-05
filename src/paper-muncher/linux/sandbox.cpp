module;

#include <fcntl.h>
#include <karm/macros>
#include <linux/landlock.h>
#include <seccomp.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <unistd.h>

module PaperMuncher;

import Karm.Sys.Posix;

using namespace Karm;

namespace PaperMuncher {

#define FOREACH_SYSCALLS(SYSCALL) \
    SYSCALL(access)               \
    SYSCALL(brk)                  \
    SYSCALL(close)                \
    SYSCALL(exit_group)           \
    SYSCALL(fstat)                \
    SYSCALL(futex)                \
    SYSCALL(getcwd)               \
    SYSCALL(getdents64)           \
    SYSCALL(lseek)                \
    SYSCALL(mmap)                 \
    SYSCALL(mprotect)             \
    SYSCALL(munmap)               \
    SYSCALL(newfstatat)           \
    SYSCALL(fstat)                \
    SYSCALL(openat)               \
    SYSCALL(read)                 \
    SYSCALL(write)                \
    /* for uti */                 \
    SYSCALL(getrandom)            \
    /* for async runtime */       \
    SYSCALL(io_uring_enter)       \
    SYSCALL(io_uring_setup)       \
    SYSCALL(epoll_ctl)            \
    SYSCALL(epoll_pwait)          \
    /* for libunwind */           \
    SYSCALL(rt_sigprocmask)       \
    SYSCALL(pipe2)                \
    SYSCALL(getpid)               \
    SYSCALL(gettid)               \
    SYSCALL(tgkill)

static Res<> _landlockInDirectory(Vec<Str> dirs) {
    landlock_ruleset_attr attr = {};
    attr.handled_access_fs = LANDLOCK_ACCESS_FS_READ_FILE | LANDLOCK_ACCESS_FS_WRITE_FILE;

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
            .allowed_access = attr.handled_access_fs,
            .parent_fd = dirFd
        };

        if (syscall(__NR_landlock_add_rule, rulesetFd, LANDLOCK_RULE_PATH_BENEATH, &pathAttr, 0) < 0)
            return Posix::fromLastErrno();
    }

    if (syscall(__NR_landlock_restrict_self, rulesetFd, 0) < 0)
        return Posix::fromLastErrno();

    return Ok();
}

Res<> hardenSandbox() {
    // MARK: RLimit
    constexpr rlimit rl{.rlim_cur = 4_GiB, .rlim_max = 4_GiB};
    if (setrlimit(RLIMIT_AS, &rl) < 0)
        return Posix::fromLastErrno();

    // https://www.kernel.org/doc/Documentation/prctl/no_new_privs.txt
    if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) < 0)
        return Posix::fromLastErrno();

    // MARK: Landlock
    auto myProc = "/proc/{}"_f(getpid()); // for libunwind
    auto [repo, _] = try$(Posix::repoRoot()); // for bundle:// urls
    try$(_landlockInDirectory({repo, myProc}));

    // MARK: Seccomp
    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_KILL_PROCESS);
    if (!ctx)
        return Posix::fromLastErrno();
    Defer _{[&] {
        seccomp_release(ctx);
    }};

#define ITER(SYSCALL)                                                                  \
    if (auto it = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(SYSCALL), 0); it < 0) \
        return Posix::fromErrno(-it);
    FOREACH_SYSCALLS(ITER)
#undef ITER

    if (seccomp_load(ctx) < 0)
        return Posix::fromLastErrno();

    return Ok();
}

} // namespace PaperMuncher
