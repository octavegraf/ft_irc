/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_signal_action.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchan-re <rchan-re@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 13:46:12 by rchan-re          #+#    #+#             */
/*   Updated: 2026/02/09 16:05:37 by rchan-re         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
