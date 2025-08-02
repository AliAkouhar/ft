#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"

/*
 * Parameters: <channel> [<topic>]
 * Reference: https://datatracker.ietf.org/doc/html/rfc1459#section-4.2.4
 */

std::string extract_topic(const std::string& buffer, const std::string& channel_name)
{
	size_t pos = buffer.find(channel_name);
    pos = buffer.find(' ', pos);
    if (pos == std::string::npos) return NULL;

	return (buffer.substr(pos + 1));
}

void Server::_ft_topic(const std::string& buffer, const int fd) {
	std::vector<std::string> params = _split_buffer(buffer, SPACE);
	std::string channel_name = params[0];
	Client* client = _get_client(fd);

	Channel* channel = _get_channel(channel_name);
	if (_topic_checks(client, fd, channel_name, params)) {
		return;
	}

	if (params.size() == 1)
	{
		std::string topic = channel->get_topic();
		if (topic.empty()) {
			_send_response(fd, RPL_NOTOPIC(client->get_nickname(), channel_name));
			_reply_code = 331;
		} else {
			_send_response(fd, RPL_TOPIC(client->get_nickname(), channel_name, topic));
			_reply_code = 332;
		}
		return;
	}
	
	if (channel->get_topic_restriction() && !channel->is_channel_operator(client->get_nickname())) {
		_send_response(fd, ERR_CHANOPRIVSNEEDED(channel_name));
		_reply_code = 482;
		return;
	}
	std::string new_topic = extract_topic(buffer, channel_name);

	channel->set_topic(new_topic);
	_send_response(fd, RPL_TOPIC(client->get_nickname(), channel_name, new_topic));
		_reply_code = 332;
}
