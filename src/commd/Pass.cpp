#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"

void Server::_handler_client_password(const std::string& buffer, const int fd)
{
    Client* client = _get_client(fd);

    if (buffer.empty())
    {
        _send_response(fd, ERR_NEEDMOREPARAMS(std::string("*")));
		_reply_code = 461;
    }
    else if (client->get_is_authenticated())
    {
        _send_response(fd, ERR_ALREADYREGISTERED(client->get_nickname()));
		_reply_code = 462;
    }
    else if (_pass != buffer)
	{
		_send_response(fd, ERR_INCORPASS(std::string("*")));
		_reply_code = 464;
	}
	else
	{
		client->set_is_authenticated(true);
		_reply_code = 200;
	}
}
