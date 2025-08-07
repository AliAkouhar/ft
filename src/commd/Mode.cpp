#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"


/*
 * Parameters: <channel> {[+|-]|o|p|s|i|t|n|b|v} [<limit>] [<user>]
 * Reference: https://datatracker.ietf.org/doc/html/rfc1459#section-4.2.3
 */

void Server::_ft_mode(const std::string& buffer, const int fd) {
	std::string channelName, modeFlags, argument;
	std::istringstream iss(buffer);

	iss >> channelName >> modeFlags;
	std::getline(iss >> std::ws, argument);

	Client* client = _get_client(fd);
	if (_mode_checks(client, fd, channelName, modeFlags))
		return ;
	
	Channel* channel = _get_channel(channelName);
	if (!_process_flags(
				 modeFlags, channel, _get_client(argument), argument))
	{
		_send_response(
			fd,
			ERR_UNKNOWNMODE(
				client->get_nickname(), channel->get_name(), modeFlags[1]));
		_reply_code = 472;
		return ;
	}
	
	_send_response(fd,
					RPL_UMODEIS(client->get_nickname(),
								client->get_hostname(),
								channel->get_name(),
								modeFlags[0],
								modeFlags[1],
								argument));
	_reply_code = 200;
}
