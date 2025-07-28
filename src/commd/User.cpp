#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"

void Server::_handler_client_username(const std::string& buffer, const int fd)
{
	Client* client = _get_client(fd);

	if (buffer.empty())
	{
		_send_response(fd, ERR_NEEDMOREPARAMS(std::string("*")));
		_reply_code = 461;
	}
	else if (!client || !client->get_is_authenticated())
	{
		_send_response(fd, ERR_NOTREGISTERED(std::string("*")));
		_reply_code = 451;
	}
	else if (!client->get_already_registered())
	{
		_send_response(fd, ERR_ALREADYREGISTERED(client->get_nickname()));
		_reply_code = 462;
	}
	else
	{
		client->set_username(buffer);
		if (_client_is_ready_to_login(fd))
		{
			client->set_is_logged(fd); 	
			_send_response(fd,
						   RPL_CONNECTED(_get_hostname(),
										 client->get_nickname(),
										 client->get_hostname()));
			_reply_code = 001;
		}
		else
		{
			_reply_code = 200;
		}
	}
}
