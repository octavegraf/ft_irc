/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_socket.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchan-re <rchan-re@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 10:43:48 by rchan-re          #+#    #+#             */
/*   Updated: 2026/02/05 14:25:43 by rchan-re         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <poll.h>
#define BUFFER_SIZE 1024

static void	print_sockaddr(struct sockaddr *addr, socklen_t len)
{
	printf("sockaddr: ");
	for (unsigned int i = 0; i < len && i < 14; i++)
	{
		printf("%u.", addr->sa_data[i]);
	}
	printf("\n");
}


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
	fflush(stdout);
}

static void	fill_getaddrinfo_hints(struct addrinfo *hints, int ai_flags, int ai_family, int ai_socktype, int ai_protocol)
{
	hints->ai_flags = ai_flags;
	hints->ai_family = ai_family;
	hints->ai_socktype = ai_socktype;
	hints->ai_protocol = ai_protocol;
	hints->ai_addrlen = 0;
	hints->ai_addr = NULL;
	hints->ai_canonname = NULL;
	hints->ai_next = NULL;
}

static void	init_sockaddr(struct sockaddr *s, socklen_t *len)
{
	s->sa_family = 0;
	for (int i = 0; i < 14; i++)
		s->sa_data[i] = 42;
	*len = 14;
}

int	main(int argc, char **argv)
{
	struct addrinfo	*res;
	struct addrinfo	*ptr;
	struct addrinfo	hints;
	int				sfd;
	char			buffer[BUFFER_SIZE];
	int				nbytes;
	struct sockaddr	addr;
	socklen_t		len;
	int				read_sfd;
//	int				write_sfd;
	int				val;
	struct pollfd	sfds[2];

	if (argc != 3)
		return (1);
	// get addr info: ircd 6667
	//fill_getaddrinfo_hints(&hints, 0, AF_UNSPEC, 0, 17); // udp
	//fill_getaddrinfo_hints(&hints, 0, AF_UNSPEC, 0, 6); // tcp
	//fill_getaddrinfo_hints(&hints, 0, AF_UNSPEC, AF_INET, 6); // tcp ipv4
	//fill_getaddrinfo_hints(&hints, 0, AF_UNSPEC, AF_INET6, 6); // tcp ipv6
	fill_getaddrinfo_hints(&hints, 0, AF_UNSPEC, AF_LOCAL, 6); // tcp local
	//fill_getaddrinfo_hints(&hints, 0, AF_UNSPEC, 0, 0); // ip
	//fill_getaddrinfo_hints(&hints, 0, 0, 0, 0); // no hints
	val = getaddrinfo(argv[1], argv[2], &hints, &res);
	if (val != 0)
		return (printf("getaddrinfo(): %s\n", gai_strerror(val)), 1);
	ptr = res;
	// attempt socket connection: socket() + bind()
	while (ptr != NULL)
	{
		addr_print_info(ptr);
		sfd = socket(ptr->ai_family, ptr->ai_socktype | SOCK_NONBLOCK, ptr->ai_protocol);
		if (sfd == -1)
			return (freeaddrinfo(res), perror("socket(): "), 1);
		if (bind(sfd, ptr->ai_addr, ptr->ai_addrlen) == 0) // check errno in case of fail: assignment in progress etc.
			break ;
		close(sfd);
		ptr = ptr->ai_next;
	}
	if (ptr == NULL)
	{
		freeaddrinfo(res);
		printf("Unable to bind to port.\n");
		return (1);
	}
	// listen: only for connection mode
	if (ptr->ai_protocol == 6 && listen(sfd, 0) != 0)
		return (freeaddrinfo(res), perror("listen(): "), 1);
	// receive: check flags (MSG_WAITALL); use MSG_PEEK in case of buffer too short
	// accept?
	read_sfd = sfd;
	init_sockaddr(&addr, &len);
//	print_sockaddr(&addr, len);
	int n = 0;
	while (n < 2)
	{
		if (ptr->ai_protocol == 6)
		{
			read_sfd = accept(sfd, &addr, &len);
//			while (read_sfd == sfd || (read_sfd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)))
			while (read_sfd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
				read_sfd = accept(sfd, &addr, &len);
			if (read_sfd == -1)
				return (close(sfd), freeaddrinfo(res), perror("accept(): "), 1);
			print_sockaddr(&addr, len);
			if (fcntl(read_sfd, F_SETFL, O_NONBLOCK) == -1)
				return (close(sfd), close(read_sfd), freeaddrinfo(res), perror("accept(): "), 1);
		}
		sfds[n].fd = read_sfd;
//		sfds[n].events = POLLIN | POLLPRI | POLLOUT;
		sfds[n].events = POLLIN | POLLOUT;
		sfds[n].revents = 0;
		n++;
	}
/*	write_sfd = socket(ptr->ai_family, ptr->ai_socktype | SOCK_NONBLOCK, ptr->ai_protocol); // NONBLOCK flag + poll check for writing
	if (write_sfd == -1)
		return (close(sfd), close(read_sfd), freeaddrinfo(res), perror("write socket(): "), 1);
	sleep(5);
	if (connect(write_sfd, &addr, len) != 0)
		return (close(sfd), close(read_sfd), freeaddrinfo(res), perror("write connect(): "), 1);
	if (getsockname(write_sfd, &addr, &len) != 0)
		return (close(sfd), close(read_sfd), freeaddrinfo(res), perror("getsockname(): "), 1);
	print_sockaddr(&addr, len);*/
	freeaddrinfo(res);
	// recv?
	//printf("sfd: %d, read_sfd: %d, write_sfd: %d\n", sfd, read_sfd, write_sfd);
	printf("sfd: %d, read_sfd: %d\n", sfd, read_sfd);
	fflush(stdout);
	//sleep(10);



	while (1)
	{
		int	res_poll = poll(sfds, 2, 0);
		if (res_poll == -1)
			return (perror("poll(): "), 1); // exit properly
//		printf("res_poll: %d\n", res_poll);
		for (int i=0; i < 2; i++)
		{
//			if ((sfds[i].revents | POLLIN) == POLLIN || (sfds[i].revents | POLLPRI) == POLLPRI)
			if ((sfds[i].revents & POLLIN) == POLLIN)
			{
				nbytes = recv(sfds[i].fd, buffer, BUFFER_SIZE, 0);
				while (nbytes > 0) // \n\r?
				{
					//printf("nbytes: %d\n", nbytes);
					write(1, buffer, nbytes);
					nbytes = recv(sfds[i].fd, buffer, BUFFER_SIZE, 0);
				}
				//if ((sfds[i].revents & POLLOUT) == POLLOUT && printf("%d\n", i) && (send(sfds[i].fd, "H\n", 2, 0) == -1) )
				if ((sfds[i].revents & POLLOUT) == POLLOUT && (send(sfds[i].fd, "H\n", 2, 0) == -1) )
				//if ((send(read_sfd, "H", 1, 0) == -1) && printf("%d\n", i))
					//return (close(sfd), close(read_sfd), close(write_sfd), freeaddrinfo(res), perror("write send(): "), fflush(stdout), 1);
					return (close(sfd), close(read_sfd), freeaddrinfo(res), perror("write send(): "), fflush(stdout), 1);
			}
		}
	}

	write(1, "\n", 1);
	close(sfd);
	close(read_sfd);
	//close(write_sfd);
	
}
