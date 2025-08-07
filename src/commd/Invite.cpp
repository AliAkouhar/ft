#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"

/*
 * Parameters: <nickname> <channel>
 * Link: https://datatracker.ietf.org/doc/html/rfc1459#section-4.2.7
 */

void Server::_ft_invite(const std::string& buffer, const int fd) {
	std::vector<std::string> params = _split_buffer(buffer, SPACE);
	Client* client = _get_client(fd);

	std::string target_nick = params[0];
	Client* invitee = _get_client(target_nick);
	if (_invite_checks(client ,invitee, fd, params)) {
		return;
	}
	std::string target_channel = params[1];

	invitee->add_channel_invited(target_channel);

	_send_response(fd, RPL_INVITING(client->get_hostname(),
										target_channel,
										client->get_nickname(),
										target_nick));
	_send_response(invitee->get_fd(), RPL_INVITING(invitee->get_hostname(),
										target_channel,
										invitee->get_nickname(),
										target_nick));
	_reply_code = 200;
}
