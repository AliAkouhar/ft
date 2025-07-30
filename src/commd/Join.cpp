#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"

void Server::_ft_join(const std::string& buffer, const int fd) {
	std::vector<std::string> params = _split_buffer(buffer, SPACE);
	std::string channel_name = params[0];
	Client* client = _get_client(fd);

	if (_join_checks(client, fd, params)) {
		return ;	
	}

	Channel* channel = _get_channel(channel_name);

	if (!channel) {
		channel = new Channel(channel_name);
		_add_channel(channel);
		channel->join(client);
		channel->set_channel_operator(client);
	}
	else 
	{
		if (channel->has_client(client)) {
			_send_response(fd, ERR_ALREADYREGISTERED(client->get_nickname()));
			_reply_code = 462;
			return;
		}

		if (channel->is_channel_invite_only() &&
			!client->is_channel_invited(channel_name)) {
			_send_response(fd, ERR_INVITEONLYCHAN(client->get_nickname(), channel_name));
			_reply_code = 473;
			return;
		}
		channel->join(client);
	}
    
	_send_response(fd, RPL_JOINMSG(client->get_nickname(), client->get_hostname(), channel_name));
	_reply_code = 200;
}
