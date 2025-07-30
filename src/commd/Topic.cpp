#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"

/*
 * Parameters: <channel> [<topic>]
 * Reference: https://datatracker.ietf.org/doc/html/rfc1459#section-4.2.4
 */

void Server::_ft_topic(const std::string& buffer, const int fd) {
	std::vector<std::string> params = _split_buffer(buffer, SPACE);
	std::string channel_name = params[0];
	Client* client = _get_client(fd);

	Channel* channel = _get_channel(channel_name);
	if (_topic_checks(client, fd, channel_name)) {
		return;
	}

	if (params.size() == 1)
	{
		std::string topic = channel->get_topic();
		if (topic.empty()) {
			_send_response(fd, RPL_NOTOPIC(client->get_nickname(), channel_name));
		} else {
			_send_response(fd, RPL_TOPIC(client->get_nickname(), channel_name, topic));
		}
		_reply_code = 331;
	}
	else
	{
		if (channel->get_topic_restriction() && !channel->is_channel_operator(client->get_nickname())) {
			_send_response(fd, ERR_CHANOPRIVSNEEDED(channel_name));
			_reply_code = 482;
			return;
		}

		std::string new_topic = buffer.substr(buffer.find(params[1]));
		channel->set_topic(new_topic);
		_send_response(fd, RPL_TOPIC(client->get_nickname(), channel_name, new_topic));
			_reply_code = 332;
	}
}

// if (params.size() < 1) {
	// 	_send_response(fd, ERR_NEEDMOREPARAMS(client->get_nickname()));
	// 	_reply_code = 461;
	// 	return;
	// }

	// if (!client->get_is_logged()) {
	// 	_send_response(fd, ERR_NOTREGISTERED(client->get_nickname()));
	// 	_reply_code = 451;
	// 	return;
	// }

	// Channel* channel = _get_channel(channel_name);

	// if (!channel) {
	// 	_send_response(fd, ERR_NOSUCHCHANNEL(channel_name));
	// 	_reply_code = 403;
	// 	return;
	// }

	// if (!channel->is_client_in_channel(client->get_nickname())) {
	// 	_send_response(fd, ERR_NOTONCHANNEL(channel_name));
	// 	_reply_code = 442;
	// 	return;
	// }