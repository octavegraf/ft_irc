/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_gethostbyname.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchan-re <rchan-re@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 13:44:51 by rchan-re          #+#    #+#             */
/*   Updated: 2026/01/27 15:21:21 by rchan-re         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <netdb.h>
#include <stdio.h>

int	main(void)
{
	struct hostent	*ptr;
	int				i;

	//ptr = gethostbyname("salut"); // KO
	//ptr = gethostbyname("google.com"); // OK
	//ptr = gethostbyname("google."); // KO
	//ptr = gethostbyname("google"); // KO
	ptr = gethostbyname("localhost"); // OK
	//ptr = gethostbyname("rchan-re"); // KO
	//ptr = gethostbyname("127.0.0.1"); // OK
	//ptr = gethostbyname("8.8.8.8"); // OK
	//ptr = gethostbyname("10.8.8.8"); // OK
	//ptr = gethostbyname("256.8.8.8"); // KO
	if (ptr != NULL)
	{
		printf("h_name: %s\n", ptr->h_name);
		i = 0;
		while (ptr->h_aliases[i] != NULL)
		{
			printf("h_alias: %s\n", ptr->h_aliases[i]);
			i++;
		}
		printf("h_addrtype: %d\n", ptr->h_addrtype);
		printf("h_length: %d\n", ptr->h_addrtype);
		i = 0;
		while (ptr->h_aliases[i] != NULL)
		{
			printf("h_addr_list: %s\n", ptr->h_addr_list[i]);
			i++;
		}
	}

}
