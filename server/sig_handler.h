
int catch_signal(int sig, void (*handler)(int));
void handler(int sig, siginfo_t *siginfo, void *context);
