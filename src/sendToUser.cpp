#include "User.hpp"
#include <poll.h>
#include <sys/socket.h>

void	sendToUser(const char *buffer, size_t length, const User& user)
{
	struct pollfd	send_pollfd;

	send_pollfd.fd = user.getSfd();
	send_pollfd.events = POLLOUT;
	send_pollfd.revents = 0;
	int	res_poll = poll(&send_pollfd, 1, 0);
	while (res_poll == 0)
		res_poll = poll(&send_pollfd, 1, 0);
	if (res_poll == -1)
		throw std::exception();
	if (send(send_pollfd.fd, buffer, length, 0) == -1)
		throw std::exception();
}
