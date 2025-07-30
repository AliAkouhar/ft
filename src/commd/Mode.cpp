#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"

void Server::_ft_mode(const std::string& buffer, const int fd) {
	std::string channelName, modeFlags, argument;
	std::istringstream iss(buffer);

	iss >> channelName >> modeFlags;
	if (iss >> argument)
		iss >> argument;

	if (modeFlags.empty())
	{
		_reply_code = 461;
		return;
	}

	Client* client = _get_client(fd);
	if (channelName.empty() || modeFlags.empty())
	{
		_send_response(fd, ERR_NEEDMOREPARAMS(client->get_nickname()));
		_reply_code = 461;
		return ;
	}

	Channel* channel = _get_channel(channelName);
	if (!channel)
	{
		_send_response(fd, ERR_NOSUCHCHANNEL(channelName));
		_reply_code = 403;
		return ;
	}

	if (!channel->is_channel_operator(client->get_nickname()))
	{
		_send_response(fd, ERR_CHANOPRIVSNEEDED(channelName));
		_reply_code = 482;
		return ;
	}

	if (!_process_mode_flags(
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
