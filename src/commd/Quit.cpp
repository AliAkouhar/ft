#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"

void Server::_handler_client_quit(const std::string& buffer, const int fd) {
	(void)buffer; // Suppress unused parameter warning
	(void)fd;     // Suppress unused parameter warning

	// Client* client = _get_client(fd);

    // for (std::vector<Channel*>::iterator it = _channels.begin();
	// 	 it != _channels.end();
	// 	 ++it)
	// {
	// 	Channel* channel = *it;
	// 	if (channel->has_client(client))
	// 	{
	// 		channel->broadcast(client, channel->get_name(), "good bye !");
	// 		channel->quit(client);
	// 	}
	// }
	// _send_response(fd, RPL_QUITMESSAGE(client->get_nickname()));
	// _reply_code = 301;

	// std::cout << RED << "Client <" << fd << "> Disconnected" << WHI
	// 		  << std::endl;
	// _clear_client(fd);
}
