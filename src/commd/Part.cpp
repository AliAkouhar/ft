#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"

void Server::_ft_part(const std::string& buffer, const int fd) {
	Client* client = _get_client(fd);
    std::vector<std::string> param = _split_buffer(buffer, SPACE);
    std::string nickname = client->get_nickname();
    std::string channel_name = param[0];

    if (buffer.empty())
    {
        _send_response(fd, ERR_NEEDMOREPARAMS(client->get_nickname()));
        _reply_code = 461;
        return ;
    }

    if (!client->get_is_logged())
    {
        _send_response(fd, ERR_USERNOTINCHANNEL(nickname, param[0]));
        _reply_code = 451;
        return ;
    }

    _part_cont(channel_name, client, fd);
}

    // {

	// 	Channel* channel = _get_channel(channel_name);
	// 	if (!channel)
	// 	{
	// 		_send_response(fd, ERR_NOSUCHCHANNEL(channel_name));
	// 		_reply_code = 403;
	// 	}
	// 	else if (!channel->has_client(client))
	// 	{
	// 		_send_response(fd, ERR_NOTONCHANNEL(channel_name));
	// 		_reply_code = 442;
	// 	}
	// 	else
	// 	{
	// 		channel->part(client);
	// 		_send_response(fd,
	// 					   RPL_PART(client->get_hostname(),
	// 								channel_name,
	// 								client->get_nickname()));
	// 		_reply_code = 200;
	// 	}
	// }