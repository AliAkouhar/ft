#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"


/*
 * Parameters: <channel> [<key>]
 * Link: https://datatracker.ietf.org/doc/html/rfc1459#section-4.2.1
 */

void Server::_ft_join(const std::string& buffer, const int fd) {
	std::vector<std::string> params = _split_buffer(buffer, SPACE);
	std::string channel_name = params[0];

	Client* client = _get_client(fd);
	if (buffer.empty())
	{
		_send_response(fd, ERR_NEEDMOREPARAMS(client->get_nickname()));
		_reply_code = 461;
		return;
	}
	
	if (!client->get_is_logged())
	{
		_send_response(fd, ERR_NOTREGISTERED(client->get_nickname()));
		_reply_code = 451;
		return;
	}
	
	if (_join_checks(client, fd, params)) {
		return ;	
	}

	Channel* channel = _get_channel(channel_name);
	channel->join(client);
	_send_response(fd,
				RPL_JOINMSG(client->get_nickname(),
							client->get_hostname(),
							channel_name));
	_reply_code = 200;
}
