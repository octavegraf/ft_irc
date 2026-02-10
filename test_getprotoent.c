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
