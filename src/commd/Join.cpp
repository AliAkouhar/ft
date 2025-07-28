#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"

void Server::_handler_client_join(const std::string& buffer, const int fd) {
	std::vector<std::string> params = _split_buffer(buffer, SPACE);
	Client* client = _get_client(fd);

	if (params.empty()) {
		_send_response(fd, ERR_NEEDMOREPARAMS(client->get_nickname()));
		_reply_code = 461;
		return;
	}
	if (!client->get_is_logged()) {
		_send_response(fd, ERR_NOTREGISTERED(client->get_nickname()));
		_reply_code = 451;
		return;
	}

	std::string channel_name = params[0];
	std::string channel_key = (params.size() > 1) ? params[1] : "";

	if (channel_name.empty() || channel_name[0] != '#') {
		_send_response(fd, ERR_BADCHANMASK(_get_hostname(), client->get_nickname(), channel_name));
		_reply_code = 403;
		return;
	}

	Channel* channel = _get_channel(channel_name);

	if (!channel) {
		channel = new Channel(channel_name);
		_add_channel(channel);
		channel->join(client);
		channel->set_channel_operator(client);
	} else {
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
