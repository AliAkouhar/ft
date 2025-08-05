#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"

/* KICK <channel> <user> [:<comment>] */

void Server::_ft_kick(const std::string& buffer, const int fd) {
	std::vector<std::string> params = _split_buffer(buffer, SPACE);

	std::string channel_name = params[0];

	Client* kicker = _get_client(fd);
	Channel* channel = _get_channel(channel_name);
	if (_kick_checks(kicker, channel, fd, channel_name, params))
	{
		return;
	}
	
	std::vector<std::string> comments = _split_buffer(params[1], SPACE);
	
	std::string target_nickname = comments[0];
	Client* target = _get_client(target_nickname);
	if (!target || !channel->has_client(target))
	{
		_send_response(fd, ERR_USERNOTINCHANNEL(target_nickname, channel_name));
		_reply_code = 441;
		return;
	}
	
	std::string reason = (comments.size() > 1) ? comments[1] : std::string("");
	std::string kick_msg = RPL_KICK(
			kicker->get_hostname(),
			channel_name,
			kicker->get_nickname(),
			target->get_nickname(),
			reason
	);

	_send_response(target->get_fd(), kick_msg);

	channel->kick(target);
	_reply_code = 200;
}
