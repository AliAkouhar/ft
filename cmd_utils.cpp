#include "../inc/Server.hpp"

int Server::_join_checks(Client *client, const int fd, const std::vector<std::string>& params) {
	std::string channel_name = params[0];
	if (params.empty()) {
		_send_response(fd, ERR_NEEDMOREPARAMS(client->get_nickname()));
		_reply_code = 461;
		return;
	}

	if (!client->get_is_logged()) {
		_send_response(fd, ERR_NOTREGISTERED(client->get_nickname()));
		_reply_code = 451;
		return;
	}

	if (channel_name.empty() || channel_name[0] != '#') {
		_send_response(fd, ERR_BADCHANMASK(_get_hostname(), client->get_nickname(), channel_name));
		_reply_code = 403;
		return;
	}
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

int Server::_user_checks(Client* client)
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