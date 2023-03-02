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
    run_process(BASH);
    umount("/proc");
    exit(EXIT_SUCCESS);
}

int main(int argc, char **agrv)
{
    printf("main process\n");
    int8_t status = clone(child_process, reserve_stack_memory(), CLONE_NEWPID | CLONE_NEWUTS | SIGCHLD, 0);
    wait(nullptr);
    return EXIT_SUCCESS;
}