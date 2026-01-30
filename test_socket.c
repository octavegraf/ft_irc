/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_socket.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchan-re <rchan-re@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 10:43:48 by rchan-re          #+#    #+#             */
/*   Updated: 2026/01/30 19:28:01 by rchan-re         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

int	main(void)
{
	struct addrinfo	*ptr;
	int				sfd;

	if (argc != 3)
		return (1);
	// get addr info: what service? ircd 6667
	// no hints for now
	if (getaddrinfo(argv[0], argv[1], NULL, ptr) != 0)
	{
		return (1);
	}
	// attempt socket connection: socket() + bind()
	// listen
	

}
