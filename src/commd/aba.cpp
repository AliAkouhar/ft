
void Server::_handler_client_privmsg(const std::string& buffer, const int fd)
{
	Client* client = _get_client(fd);

	std::vector<std::string> params = _split_buffer(buffer, " ");

	if (client->get_is_logged())
	{
		if (params.size() < 2)
		{
			_send_response(fd, ERR_NEEDMOREPARAMS(client->get_nickname()));
			_reply_code = 461;
			return;
		}

		std::vector<std::string> receivers = split_parameters(params[0], ",");

		for (std::vector<std::string>::iterator it = receivers.begin();
			 it != receivers.end();
			 ++it)
		{
			if ((*it)[0] == '#')
			{
				Channel* target_channel = this->_get_channel(*it);
				if (!target_channel)
				{
					_send_response(fd, ERR_NOSUCHCHANNEL(*it));
					_reply_code = 403;
					return;
				}

				if (!target_channel->has_client(client))
				{
					_send_response(fd,
								   ERR_NOTONCHANNEL(client->get_nickname()));
					_reply_code = 442;
					return;
				}
			}
			else
			{
				Client* target_client = this->_get_client(*it);
				if (!target_client)
				{
					_send_response(fd,
								   ERR_NOSUCHNICK(std::string(""),
												  client->get_nickname()));
					_reply_code = 401;
					return;
				}
			}
		}

		for (std::vector<std::string>::iterator it = receivers.begin();
			 it != receivers.end();
			 ++it)
		{
			if ((*it)[0] == '#')
			{
				Channel* target_channel = this->_get_channel(*it);
				target_channel->broadcast(
					client, target_channel->get_name(), params[1]);
			}
			else
			{
				Client* target_client = this->_get_client(*it);
				_send_response(target_client->get_fd(),
							   RPL_PRIVMSG(client->get_nickname(),
										   client->get_hostname(),
										   target_client->get_nickname(),
										   params[1]));
			}
		}
	}
	else
	{
		_send_response(fd, ERR_NOTREGISTERED(client->get_nickname()));
		_reply_code = 451;
	}
}