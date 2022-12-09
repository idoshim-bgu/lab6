#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>
int  flag = 0; // stop is stop, cont is cont

void handler(int sig)
{
	if (sig == SIGTSTP && flag)
		sig = SIGCONT;
	else if (sig == SIGCONT && flag)
		sig = SIGTSTP;
	printf("\nRecieved Signal : %s\n", strsignal(sig));
	if (sig == SIGTSTP || sig == SIGCONT)
		flag = 1 - flag; 
	else{
		signal(sig, SIG_DFL);
		raise(sig);
	}
	
	// signal(sig, handler);
}

int main(int argc, char **argv)
{

	printf("Starting the program\n");
	signal(SIGINT, handler);
	signal(SIGTSTP, handler);
	signal(SIGCONT, handler);

	while (1)
	{
		sleep(1);
	}

	return 0;
}