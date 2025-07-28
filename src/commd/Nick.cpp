#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"

void Server::_handler_client_nickname(const std::string& buffer, const int fd)
{
    Client* client = _get_client(fd);

    if (buffer.empty())
    {
        _send_response(fd, ERR_NEEDMOREPARAMS(std::string("*")));
		_reply_code = 461;
    }
    else if (!client->get_is_authenticated())
    {
        _send_response(fd, ERR_NOTREGISTERED(std::string("*")));
		_reply_code = 451;
    }
    else if (!_is_valid_nickname(buffer))
    {
        _send_response(fd, ERR_NONICKNAME(client->get_nickname()));
        _reply_code = 462;
    }
    else if (_is_nickname_in_use(fd, buffer))
    {
        _send_response(fd, ERR_NICKINUSE(client->get_nickname()));
		_reply_code = 462;
    }
    else
    {
        client->set_nickname(buffer);
        if (_client_is_ready_to_login(fd))
		{
			client->set_is_logged(fd);
			_send_response(fd,
						   RPL_CONNECTED(_get_hostname(),
										 client->get_nickname(),
										 client->get_hostname()));
			_reply_code = 001;
			return;
		}
		_reply_code = 200;
    }
}
