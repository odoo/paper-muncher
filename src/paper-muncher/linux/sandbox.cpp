module;

#include <fcntl.h>
#include <karm/macros>
#include <seccomp.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <unistd.h>

module PaperMuncher;

import Karm.Sys.Posix;
import PaperMuncher.Linux;

using namespace Karm;

namespace PaperMuncher {

#define FOREACH_SYSCALLS(SYSCALL)                        \
    /* Signals & Process Lifecycle */                    \
    SYSCALL(exit_group)                                  \
    SYSCALL(exit)                                        \
    SYSCALL(rt_sigreturn)                                \
    SYSCALL(restart_syscall)                             \
    SYSCALL(rt_sigprocmask)                              \
    /* Memory & Allocators */                            \
    SYSCALL(munmap)                                      \
    SYSCALL(brk)                                         \
    SYSCALL(madvise)                                     \
    /* File I/O & Stat */                                \
    /* for accessing the bundle, hardened by landlock */ \
    SYSCALL(read)                                        \
    SYSCALL(write)                                       \
    SYSCALL(close)                                       \
    SYSCALL(access)                                      \
    SYSCALL(getdents64)                                  \
    SYSCALL(fstat)                                       \
    SYSCALL(lseek)                                       \
    SYSCALL(newfstatat)                                  \
    SYSCALL(statx) /* Modern libc fallback */            \
    SYSCALL(openat)                                      \
    /* Sync & Epoll */                                   \
    SYSCALL(futex)                                       \
    SYSCALL(getcwd)                                      \
    SYSCALL(clock_gettime)                               \
    SYSCALL(getrandom)                                   \
    SYSCALL(epoll_ctl)                                   \
    SYSCALL(epoll_wait)                                  \
    SYSCALL(epoll_pwait)                                 \
    SYSCALL(pipe2)                                       \
    SYSCALL(getpid)                                      \
    SYSCALL(gettid)                                      \
    SYSCALL(tgkill)

Res<> hardenSandbox(Sandbox sandbox) {
#ifndef __ck_async_epoll__
    // SECURITY: io_uring is not compatible with seccomp, so we can't harden the sandbox when using it.
    return Error::notImplemented("sandbox hardening is supported only when using epoll as the async runtime");
#endif

    // MARK: RLimit
    // limit the process to 4GiB of memory
    rlimit rl{
        .rlim_cur = sandbox.memory,
        .rlim_max = sandbox.memory,
    };
    if (setrlimit(RLIMIT_AS, &rl) < 0)
        return Posix::fromLastErrno();

    // no core dump with document content
    constexpr rlimit noCore{
        .rlim_cur = 0,
        .rlim_max = 0,
    };
    setrlimit(RLIMIT_CORE, &noCore);

    // no fork bomb
    constexpr rlimit noProc{
        .rlim_cur = 1,
        .rlim_max = 1,
    };
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
    auto proc = "/proc/{}"_f(getpid());

    // for bundle:// urls
    auto [repo, _] = try$(Posix::repoRoot());
    try$(landlockAllowReadingDirectory({
        repo,
        proc,
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

    if (auto it = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mprotect), 1, SCMP_A2(SCMP_CMP_MASKED_EQ, PROT_EXEC, 0)); it < 0)
        return Posix::fromErrno(-it);

    if (auto it = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mmap), 1, SCMP_A2(SCMP_CMP_MASKED_EQ, PROT_EXEC, 0)); it < 0)
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
