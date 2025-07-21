#include "../../inc/Server.hpp"

void Server::_handler_client_topic(const std::string& buffer, const int fd) {
	Client* client = _get_client(fd);
	std::vector<std::string> params = _split_buffer(buffer, SPACE);

	if (params.size() < 1) {
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
	Channel* channel = _get_channel(channel_name);

	if (!channel) {
		_send_response(fd, ERR_NOSUCHCHANNEL(channel_name));
		_reply_code = 403;
		return;
	}

	if (!channel->has_client(client)) {
		_send_response(fd, ERR_NOTONCHANNEL(channel_name));
		_reply_code = 442;
		return;
	}

	// Show topic
	if (params.size() == 1) {
		std::string topic = channel->get_topic();
		if (topic.empty()) {
			_send_response(fd, RPL_NOTOPIC(client->get_nickname(), channel_name));
		} else {
			_send_response(fd, RPL_TOPIC(client->get_nickname(), channel_name, topic));
		}
		_reply_code = 331;
	}
	// Set topic
	else {
		// Optional: Check if only ops can set the topic
		if (channel->is_topic_protected() && !channel->is_operator(client)) {
			_send_response(fd, ERR_CHANOPRIVSNEEDED(client->get_nickname(), channel_name));
			_reply_code = 482;
			return;
		}

		std::string new_topic = buffer.substr(buffer.find(params[1]));
		channel->set_topic(new_topic);
		_send_response_to_channel(
			channel,
			RPL_TOPIC(client->get_nickname(), channel_name, new_topic));
		_reply_code = 200;
	}
}
