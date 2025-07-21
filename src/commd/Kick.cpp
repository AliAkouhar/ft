#include "../../inc/Server.hpp"

/* KICK <channel> <user> [:<comment>] */

void Server::_handler_client_kick(const std::string& buffer, const int fd) {
	Client* kicker = _get_client(fd);

	if (!kicker->get_is_logged()) {
		_send_response(fd, ERR_NOTREGISTERED(kicker->get_nickname()));
		_reply_code = 451;
		return;
	}

	std::vector<std::string> params = _split_buffer(buffer, SPACE);
	if (params.size() < 2) {
		_send_response(fd, ERR_NEEDMOREPARAMS(kicker->get_nickname()));
		_reply_code = 461;
		return;
	}

	std::string channel_name = params[0];
	std::vector<std::string> comments = _split_buffer(params[1], SPACE);
	std::string target_nickname = comments[0];

	Channel* channel = _get_channel(channel_name);
	if (!channel) {
		_send_response(fd, ERR_NOSUCHCHANNEL(channel_name));
		_reply_code = 403;
		return;
	}

	if (!channel->has_client(kicker)) {
		_send_response(fd, ERR_NOTONCHANNEL(channel_name));
		_reply_code = 442;
		return;
	}

	if (!channel->is_channel_operator(kicker->get_nickname())) {
		_send_response(fd, ERR_CHANOPRIVSNEEDED(channel_name));
		_reply_code = 482;
		return;
	}

	Client* target = _get_client(target_nickname);
	if (!target || !channel->has_client(target)) {
		_send_response(fd, ERR_USERNOTINCHANNEL(target_nickname, channel_name));
		_reply_code = 441;
		return;
	}

	
	if (params.size() > 2)
	{
		_send_response(fd,
					   RPL_KICK(kicker->get_hostname(),
								channel_name,
								kicker->get_nickname(),
								target->get_nickname(),
								comments[1]));
	}
	else
	{
		_send_response(fd,
					   RPL_KICK(kicker->get_hostname(),
								channel_name,
								kicker->get_nickname(),
								target->get_nickname(),
								""));
	}
	channel->kick(target);
	_reply_code = 200;
}
