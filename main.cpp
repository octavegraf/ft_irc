/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchan-re <rchan-re@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 15:59:01 by rchan-re          #+#    #+#             */
/*   Updated: 2026/02/05 16:39:10 by rchan-re         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include

int	main(int argc, char **argv)
{
	if (argc != 3)
		return (1);


	// init server: fetch socket type + create a socket + bind the socket to the host and port

	// listen routine

		// accept connection requests: create new user + update server user list + store user fd in pollfd tab

		// poll the fds to fetch new events

		// iterate on the fds to find events to handle: dispatchCommand()
}
