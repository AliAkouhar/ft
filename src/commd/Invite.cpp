#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"

void Server::_ft_invite(const std::string& buffer, const int fd) {
	Client* inviter = _get_client(fd);
	std::vector<std::string> params = _split_buffer(buffer, SPACE);

	if (!inviter->get_is_logged()) {
		_send_response(fd, ERR_NOTREGISTERED(inviter->get_nickname()));
		_reply_code = 451;
		return;
	}

	if (params.size() < 2) {
		_send_response(fd, ERR_NEEDMOREPARAMS(inviter->get_nickname()));
		_reply_code = 461;
		return;
	}

	std::string target_nick = params[0];
	std::string channel_name = params[1];

	Client* invitee = _get_client(target_nick);
	Channel* channel = _get_channel(channel_name);

	if (!invitee) {
		_send_response(fd, ERR_NOSUCHNICK(inviter->get_nickname(), target_nick));
		_reply_code = 401;
		return;
	}

	if (!channel) {
		_send_response(fd, ERR_NOSUCHCHANNEL(channel_name));
		_reply_code = 403;
		return;
	}

	if (!channel->has_client(inviter)) {
		_send_response(fd, ERR_NOTONCHANNEL(channel_name));
		_reply_code = 442;
		return;
	}

	if (channel->has_client(invitee)) {
		_send_response(fd, ERR_USERONCHANNEL(invitee->get_nickname(), channel_name));
		_reply_code = 443;
		return;
	}

	invitee->add_channel_invited(channel_name);

	_send_response(fd, RPL_INVITING(inviter->get_hostname(),
										channel_name,
										inviter->get_nickname(),
										target_nick));
			_reply_code = 200;
}
