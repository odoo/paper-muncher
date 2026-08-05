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

#define FOREACH_SYSCALLS(SYSCALL)                        \
    SYSCALL(exit_group)                                  \
    SYSCALL(exit)                                        \
    SYSCALL(futex)                                       \
    SYSCALL(getcwd)                                      \
    SYSCALL(clock_gettime)                               \
    /* for pipe/streams */                               \
    SYSCALL(read)                                        \
    SYSCALL(write)                                       \
    SYSCALL(close)                                       \
    /* for accessing the bundle, hardened by landlock */ \
    SYSCALL(access)                                      \
    SYSCALL(getdents64)                                  \
    SYSCALL(fstat)                                       \
    SYSCALL(lseek)                                       \
    SYSCALL(newfstatat)                                  \
    SYSCALL(openat)                                      \
    /* for glibc malloc */                               \
    SYSCALL(mmap)                                        \
    SYSCALL(mprotect)                                    \
    SYSCALL(munmap)                                      \
    SYSCALL(brk)                                         \
    /* for uti */                                        \
    SYSCALL(getrandom)                                   \
    /* for async runtime */                              \
    SYSCALL(epoll_ctl)                                   \
    SYSCALL(epoll_wait)                                  \
    SYSCALL(epoll_pwait)                                 \
    /* for libunwind */                                  \
    SYSCALL(rt_sigprocmask)                              \
    SYSCALL(pipe2)                                       \
    SYSCALL(getpid)                                      \
    SYSCALL(gettid)                                      \
    SYSCALL(tgkill)

static Res<> _landlockAllowReadingDirectory(Vec<Str> dirs) {
    landlock_ruleset_attr attr = {};
    attr.handled_access_fs = LANDLOCK_ACCESS_FS_READ_FILE;

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
    #ifndef __ck_async_epoll__
        // SECURITY: io_uring is not compatible with seccomp, so we can't harden the sandbox when using it.
        logWarn("sandbox hardening is supported only when using epoll as the async runtime.");
        return Ok();
    #endif

    // MARK: RLimit

    // limit the process to 4GiB of memory
    constexpr rlimit rl{.rlim_cur = 4_GiB, .rlim_max = 4_GiB};
    if (setrlimit(RLIMIT_AS, &rl) < 0)
        return Posix::fromLastErrno();

    // no core dump with document content
    constexpr rlimit noCore{.rlim_cur = 0, .rlim_max = 0};
    setrlimit(RLIMIT_CORE, &noCore);

    // no fork bomb
    constexpr rlimit noProc{.rlim_cur = 1, .rlim_max = 1};
    setrlimit(RLIMIT_NPROC, &noProc);

    // https://www.kernel.org/doc/Documentation/prctl/no_new_privs.txt
    if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) < 0)
        return Posix::fromLastErrno();

    // MARK: Landlock
    // https://docs.kernel.org/userspace-api/landlock.html
    // SECURITY: Landlock only apply to the current thread and it's children.
    //           So, if we ever add support for threading, we will have to move
    //           the landlock setup before the setup of the eventloop.

    // for libunwind
    auto myProc = "/proc/{}"_f(getpid());

    // for bundle:// urls
    auto [repo, _] = try$(Posix::repoRoot());
    try$(_landlockAllowReadingDirectory({
        repo,
        myProc,
    }));

    // MARK: Seccomp
    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_KILL_PROCESS);
    if (!ctx)
        return Posix::fromLastErrno();
    Defer _{[&] {
        seccomp_release(ctx);
    }};

    // synchronize the filters across all threads
    // https://man7.org/linux/man-pages/man3/seccomp_attr_set.3.html#:~:text=SCMP_FLTATR_CTL_TSYNC
    if (auto it = seccomp_attr_set(ctx, SCMP_FLTATR_CTL_TSYNC, 1); it < 0)
        return Posix::fromErrno(-it);

#define ITER(SYSCALL)                                                                  \
    if (auto it = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(SYSCALL), 0); it < 0) \
        return Posix::fromErrno(-it);
    FOREACH_SYSCALLS(ITER)
#undef ITER

    if (auto it = seccomp_load(ctx); it < 0)
        return Posix::fromErrno(-it);

    return Ok();
}

} // namespace PaperMuncher
