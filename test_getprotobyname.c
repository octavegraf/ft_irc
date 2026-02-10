#include <stdio.h>
#include <netdb.h>

int	main(int argc, char **argv)
{
	struct protoent	*ptr;
	int				i;

	ptr = getprotobyname(argv[argc - 1]);
	//ptr = getprotobyname("udp");
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
	endprotoent();
}
