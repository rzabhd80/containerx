#ifndef CONTAINERX_H

#define CONTAINERX_H

#include <iostream>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
char *reserve_stack_memory();
int child_process(void *agrs);
int run_process(const char *processName);
void change_root(const char *fileName);
#endif
