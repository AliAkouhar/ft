#include "Server.hpp"

void Server::_handler_client_quit(const std::string& buffer, const int fd) {
	Client* client = _get_client(fd);

    std::string quit_message = "good bye!";
	if (!buffer)
        quit_message = buffer;

    for (std::vector<Channel*>::iterator it = _channels.begin();
		 it != _channels.end();
		 ++it)
	{
		Channel* channel = *it;
		if (channel->has_client(client))
        {
            channel->broadcast(quit_message, client);
			channel->quit(client);
		}
	}

	_send_response(fd, RPL_QUITMESSAGE(client->get_nickname()));
	_reply_code = 301;

	std::cout << RED << "Client <" << fd << "> Disconnected" << WHI
			  << std::endl;
	_clear_client(fd);
}
