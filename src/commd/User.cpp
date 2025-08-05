#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"

void Server::_ft_username(const std::string& buffer, const int fd)
{
	Client* client = _get_client(fd);

	if (buffer.empty())
	{
		_send_response(fd, ERR_NEEDMOREPARAMS(std::string("*")));
		_reply_code = 461;
		return ;
	}
	else if (_user_checks(client, fd))
		return;
	
	client->set_username(buffer);
	if (_client_is_ready_to_login(fd))
	{
		client->set_is_logged(fd); 	
		_send_response(fd,
						RPL_CONNECTED(_get_hostname(),
										client->get_nickname(),
										client->get_hostname()));
		_reply_code = 001;
		return ;
	}
	_reply_code = 200;
}
