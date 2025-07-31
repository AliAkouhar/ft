#include "../inc/Server.hpp"

int Server::_mode_checks(Client* client, const int fd, const std::string& channelName,
						const std::string& modeFlags)
{
	if (modeFlags.empty())
	{
		_reply_code = 461;
		return 1;
	}

	if (channelName.empty() || modeFlags.empty())
	{
		_send_response(fd, ERR_NEEDMOREPARAMS(client->get_nickname()));
		_reply_code = 461;
		return 1;
	}

	Channel* channel = _get_channel(channelName);
	if (!channel)
	{
		_send_response(fd, ERR_NOSUCHCHANNEL(channelName));
		_reply_code = 403;
		return 1;
	}

	if (!channel->is_channel_operator(client->get_nickname()))
	{
		_send_response(fd, ERR_CHANOPRIVSNEEDED(channelName));
		_reply_code = 482;
		return 1;
	}
	return 0;
}

int Server::_join_checks(Client *client, const int fd, const std::vector<std::string>& params) {
	std::string channel_name = params[0];
	if (params.empty()) {
		_send_response(fd, ERR_NEEDMOREPARAMS(client->get_nickname()));
		_reply_code = 461;
		return 1;
	}

	if (!client->get_is_logged()) {
		_send_response(fd, ERR_NOTREGISTERED(client->get_nickname()));
		_reply_code = 451;
		return 1;
	}

	if (channel_name.empty() || channel_name[0] != '#') {
		_send_response(fd, ERR_BADCHANMASK(_get_hostname(), client->get_nickname(), channel_name));
		_reply_code = 403;
		return 1;
	}
	return 0;
}

int Server::_nickname_checks(const std::string& buffer, Client* client, const int fd)
{
    if (buffer.empty())
    {
        _send_response(fd, ERR_NEEDMOREPARAMS(std::string("*")));
		_reply_code = 461;
        return 1;
    }

    if (!client->get_is_authenticated())
    {
        _send_response(fd, ERR_NOTREGISTERED(std::string("*")));
		_reply_code = 451;
        return 1;
    }

    if (!_is_valid_nickname(buffer))
    {
        _send_response(fd, ERR_NONICKNAME(client->get_nickname()));
        _reply_code = 462;
        return 1;
    }

    if (_is_nickname_in_use(fd, buffer))
    {
        _send_response(fd, ERR_NICKINUSE(client->get_nickname()));
		_reply_code = 462;
        return 1;
    }

    return 0;
}

void Server::_part_cont(std::string& channel_name, Client* client, const int fd)
{
    Channel* channel = _get_channel(channel_name);

    if (!channel)
    {
        _send_response(fd, ERR_NOSUCHCHANNEL(channel_name));
        _reply_code = 403;
        return ;
    }

    if (!channel->has_client(client))
    {
        _send_response(fd, ERR_NOTONCHANNEL(channel_name));
        _reply_code = 442;
        return ;
    }

    channel->part(client);
    _send_response(fd,
                    RPL_PART(client->get_hostname(),
                            channel_name,
                            client->get_nickname()));
    _reply_code = 200;
}

int Server::_user_checks(Client* client, const int fd)
{
	if (!client || !client->get_is_authenticated())
	{
		_send_response(fd, ERR_NOTREGISTERED(std::string("*")));
		_reply_code = 451;
		return 1;
	}
	
	if (!client->get_already_registered())
	{
		_send_response(fd, ERR_ALREADYREGISTERED(client->get_nickname()));
		_reply_code = 462;
		return 1;
	}
	return 0;
}

int Server::_invite_checks(Client* inviter, const int fd, std::vector<std::string>& params) {

	if (params.size() < 2) {
		_send_response(fd, ERR_NEEDMOREPARAMS(inviter->get_nickname()));
		_reply_code = 461;
		return 1;
	}

	if (!inviter->get_is_logged()) {
		_send_response(fd, ERR_NOTREGISTERED(inviter->get_nickname()));
		_reply_code = 451;
		return 1;
	}

	std::string target_nick = params[0];
	std::string channel_name = params[1];

	Client* invitee = _get_client(target_nick);
	Channel* channel = _get_channel(channel_name);

	if (!invitee) {
		_send_response(fd, ERR_NOSUCHNICK(inviter->get_nickname(), target_nick));
		_reply_code = 401;
		return 1;
	}

	if (!channel) {
		_send_response(fd, ERR_NOSUCHCHANNEL(channel_name));
		_reply_code = 403;
		return 1;
	}

	if (!channel->has_client(inviter)) {
		_send_response(fd, ERR_NOTONCHANNEL(channel_name));
		_reply_code = 442;
		return 1;
	}

	if (channel->has_client(invitee)) {
		_send_response(fd, ERR_USERONCHANNEL(invitee->get_nickname(), channel_name));
		_reply_code = 443;
		return 1;
	}
	return 0;
}

int Server::_kick_checks(Client* kicker, Channel* channel, const int fd, const std::string& channel_name, std::vector<std::string> params) {
	if (params.size() < 2)
	{
		_send_response(fd, ERR_NEEDMOREPARAMS(kicker->get_nickname()));
		_reply_code = 461;
		return 1;
	}

	if (!kicker->get_is_logged())
	{
		_send_response(fd, ERR_NOTREGISTERED(kicker->get_nickname()));
		_reply_code = 451;
		return 1;
	}
	if (!channel)
	{
		_send_response(fd, ERR_NOSUCHCHANNEL(channel_name));
		_reply_code = 403;
		return 1;
	}

	if (!channel->has_client(kicker))
	{
		_send_response(fd, ERR_NOTONCHANNEL(channel_name));
		_reply_code = 442;
		return 1;
	}

	if (!channel->is_channel_operator(kicker->get_nickname()))
	{
		_send_response(fd, ERR_CHANOPRIVSNEEDED(channel_name));
		_reply_code = 482;
		return 1;
	}
	return 0;
}

int Server::_topic_checks(Client* client, const int fd, const std::string& channel_name, std::vector<std::string>& params) {
	if (params.size() < 1) {
		_send_response(fd, ERR_NEEDMOREPARAMS(client->get_nickname()));
		_reply_code = 461;
		return 1;
	}

	if (!client->get_is_logged()) {
		_send_response(fd, ERR_NOTREGISTERED(client->get_nickname()));
		_reply_code = 451;
		return 1;
	}

	Channel* channel = _get_channel(channel_name);

	if (!channel) {
		_send_response(fd, ERR_NOSUCHCHANNEL(channel_name));
		_reply_code = 403;
		return 1;
	}

	if (!channel->is_client_in_channel(client->get_nickname())) {
		_send_response(fd, ERR_NOTONCHANNEL(channel_name));
		_reply_code = 442;
		return 1;
	}
	return 0;
}

int Server::_privmsg_checks(Client* client, std::vector<std::string> receivers)
{
	if (!client->get_is_logged()) {
		_send_response(fd, ERR_NOTREGISTERED(client->get_nickname()));
		_reply_code = 451;
		return 1;
	}

	if (params.size() < 2) {
		_send_response(fd, ERR_NEEDMOREPARAMS(client->get_nickname()));
		_reply_code = 461;
		return 1;
	}

	for (std::vector<std::string>::iterator it = receivers.begin(); it != receivers.end(); ++it) {
		const std::string& target = *it;

		if (target[0] == '#') {
			Channel* channel = _get_channel(target);
			if (!channel) {
				_send_response(fd, ERR_NOSUCHCHANNEL(target));
				_reply_code = 403;
				return 1;
			}
			if (!channel->has_client(client)) {
				_send_response(fd, ERR_NOTONCHANNEL(client->get_nickname()));
				_reply_code = 442;
				return 1;
			}
		}
		else
		{
			Client* target_client = _get_client(target);
			if (!target_client) {
				_send_response(fd, ERR_NOSUCHNICK(std::string(""),
												  client->get_nickname()));
				_reply_code = 401;
				return 1;
			}
		}
	}
	return 0;
}
