/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_getaddrinfo.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchan-re <rchan-re@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 11:56:19 by rchan-re          #+#    #+#             */
/*   Updated: 2026/01/30 17:44:15 by rchan-re         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netdb.h>
#include <stdio.h>

int	main(int argc, char **argv)
{
	struct addrinfo	*ptr;

	if (argc != 3)
		return (1);
	if (getaddrinfo(argv[1], argv[2], NULL, &ptr))
		return (1);
	while (ptr != NULL)
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
		ptr = ptr->ai_next;
	}
}
