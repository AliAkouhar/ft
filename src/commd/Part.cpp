#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"

/*
 * Parameters: <channel>{,<channel>}
 * Link: https://datatracker.ietf.org/doc/html/rfc1459#section-4.2.2
 */

void Server::_ft_part(const std::string& buffer, const int fd) {
    std::vector<std::string> param = _split_buffer(buffer, SPACE);
	Client* client = _get_client(fd);

    if (buffer.empty())
    {
		_send_response(fd, ERR_NEEDMOREPARAMS(client->get_nickname()));
        _reply_code = 461;
        return ;
    }
	
	std::string channel_name = param[0];
    if (!client->get_is_logged())
    {
		_send_response(fd, ERR_USERNOTINCHANNEL(client->get_nickname(), channel_name));
        _reply_code = 451;
        return ;
    }
	
    if (!_part_cont(channel_name, client, fd))
        return ;
}
