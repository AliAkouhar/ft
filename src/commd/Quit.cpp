#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"

void Server::_ft_quit(const std::string& buffer, const int fd) {
	Client* client = _get_client(fd);
	(void)buffer;

    for (std::vector<Channel*>::iterator it = _channels.begin();
		 it != _channels.end();
		 ++it)
	{
		Channel* channel = *it;
		if (channel->has_client(client))
		{
			channel->quit(client);
		}
	}
	_send_response(fd, RPL_QUITMESSAGE(client->get_nickname()));
	_reply_code = 301;

	std::cout << RED << "Client <" << fd << "> Disconnected" << WHI
			  << std::endl;
	_clear_client(fd);
}