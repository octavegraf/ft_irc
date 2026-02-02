/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_socket.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchan-re <rchan-re@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 10:43:48 by rchan-re          #+#    #+#             */
/*   Updated: 2026/02/02 15:03:43 by rchan-re         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#define BUFFER_SIZE 10

static void	addr_print_info(struct addrinfo *ptr)
{
	printf("ai_flags: %d\n", ptr->ai_flags);
	printf("ai_family: %d\n", ptr->ai_family);
	printf("ai_socktype: %d\n", ptr->ai_socktype);
	printf("ai_protocol: %d\n", ptr->ai_protocol);
	if (ptr->ai_addr != NULL)
	{
		printf("ai_addrlen: %d\n", ptr->ai_addrlen);
		printf("ai_addr->sa_data: ");
		for (int i = 0; i < 14; i++)
			printf("%u.", ptr->ai_addr->sa_data[i]);
		printf("\n");
	}
	printf("ai_canonname: %s\n", ptr->ai_canonname);
}

int	main(int argc, char **argv)
{
	struct addrinfo	*res;
	struct addrinfo	*ptr;
	int				sfd;
	char			buffer[BUFFER_SIZE];
	int				nbytes;
	struct sockaddr	addr;
	socklen_t		len;
	int				read_sfd;

	if (argc != 3)
		return (1);
	// get addr info: what service? ircd 6667
	// no hints for now
	if (getaddrinfo(argv[1], argv[2], NULL, &res) != 0)
		return (printf("getaddrinfo()\n"), 1);
	ptr = res;
	// attempt socket connection: socket() + bind()
	while (ptr != NULL)
	{
		addr_print_info(ptr);
		sfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (sfd == -1)
			return (printf("socket()\n"), 1);
		if (bind(sfd, ptr->ai_addr, ptr->ai_addrlen) == 0) // check errno in case of fail: assignment in progress etc.
			break ;
		close(sfd);
	}
	freeaddrinfo(res);
	if (ptr == NULL)
	{
		printf("Unable to bind to port.\n");
		return (1);
	}
	// listen: only for connection mode
	if (listen(sfd, 0) != 0)
		return (printf("listen()\n"), 1);
	// receive: check flags (MSG_WAITALL); use MSG_PEEK in case of buffer too short
	// accept?
	read_sfd = accept(sfd, &addr, &len);
	if (read_sfd == -1)
		return (printf("accept()\n"), 1);
	// recv?
	printf("sfd: %d, read_sfd: %d\n", sfd, read_sfd);
	//sleep(10);
	nbytes = recv(read_sfd, buffer, BUFFER_SIZE, 0);
	while (nbytes != -1) // \n\r?
	{
		printf("nbytes: %d\n", nbytes);
		write(1, buffer, nbytes);
		nbytes = recv(read_sfd, buffer, BUFFER_SIZE, 0);
	}
	write(1, "\n", 1);
	
}
