#include <signal.h>

extern bool	interrupt;

// handle user fd closings? already done in Server destructor?
void	sigint_handler(int sig)
{
	if (sig == SIGINT)
		interrupt = true;
}

int	setup_signal_action(struct sigaction *sa)
{

	sa->sa_handler = sigint_handler;
	sa->sa_flags = 0;
	sigemptyset(&(sa->sa_mask));
	if (sigaction(SIGINT, sa, NULL) != 0)
		return (0); // throw exception instead?
	return (1);
}
