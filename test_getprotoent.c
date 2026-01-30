/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_getprotoent.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rchan-re <rchan-re@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 11:58:36 by rchan-re          #+#    #+#             */
/*   Updated: 2026/01/27 13:39:44 by rchan-re         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <netdb.h>

int	main(void)
{
	struct protoent	*ptr;
	int				i;

	while (1)
	{
		ptr = getprotoent();
		if (ptr != NULL)
		{
			printf("name: %s\n", ptr->p_name);
			i = 0;
			while (ptr->p_aliases[i] != NULL)
			{
				printf("alias: %s\n", ptr->p_aliases[i]);
				i++;
			}
			printf("number: %d\n", ptr->p_proto);
		}
		else
			return (endprotoent(), 0);
			//return (0);
	}
}
