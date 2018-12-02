#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "sig_handler.h"


// signal handler to be called on receipt of (in this case) SIGTERM
void handler(int sig, siginfo_t *siginfo, void *context)
{
  printf("PID: %ld, UID: %ld\n",
         (long) siginfo->si_pid, (long) siginfo->si_uid);
  exit(1);
}

int catch_signal(int sig, void (*handler)(int))
{
struct sigaction action;
action.sa_handler = handler;
sigemptyset(&action.sa_mask);
action.sa_flags = 0;
return sigaction (sig, &action, NULL);
}


