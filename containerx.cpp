#include "containerx.h"
#define BASH "/bin/bash"

char *reserve_stack_memory()
{
    const int stackSize = 65536;
    auto *stackPointer = new (std::nothrow) char[stackSize];
    if (stackPointer == nullptr)
    {
        printf("ERROR while reserving memory");
        exit(EXIT_FAILURE);
    }
    else
        return stackPointer + stackSize;
}

/**
 * function aims for cloning a process
 * with a size given by reserve_stack_memory function.
 * wait syscall is used to make the parent process wait for child to be terminated
 */

template <typename Function>
void clone_process(Function &&function, int flags)
{
    auto pid = clone(function, reserve_stack_memory(), flags, 0);

    wait(nullptr);
}

int run_process(const char *processName)
{
    char *const __agrs[] = {(char *)processName, (char *)0};
    return execvp(processName, __agrs);
}

void setup_env_vars()
{
    clearenv();
    setenv("TERM", "xterm-256color", 0);
    setenv("PATH", "/bin/:/sbin/:usr/bin:/usr/sbin", 0);
}

void change_root(const char *fileName)
{
    chroot(fileName);
    chdir("/");
}

int child_process(void *agrs)
{
    setup_env_vars();
    change_root("./root");
    mount("proc", "/proc", "proc", 0, 0);

    /*regardign the fact that should we run bash process on the child
     *  it will never be able to unmount proc, we use a third cloned process to
     * run bash and once its been terminated, the second process will unmount the proc
     */

    auto bash_Process = [](void *args) -> int
    { return run_process(BASH); };

    clone_process(bash_Process, SIGCHLD);
    umount("/proc");
    exit(EXIT_SUCCESS);
}

int main(int argc, char **agrv)
{
    printf("main process\n");
    clone_process(child_process, CLONE_NEWPID | CLONE_NEWUTS | SIGCHLD);

    return EXIT_SUCCESS;
}