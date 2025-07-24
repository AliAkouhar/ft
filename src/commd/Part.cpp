#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"

std::vector<std::string> _split_string(const std::string& s, char delimiter) { // add this function to  a shared file
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = s.find(delimiter);

    while (end != std::string::npos) {
        tokens.push_back(s.substr(start, end - start));
        start = end + 1;
        end = s.find(delimiter, start);
    }
    tokens.push_back(s.substr(start, std::string::npos));
    return tokens;
}

void Server::_handler_client_part(const std::string& buffer, const int fd)
{
	Client* client = _get_client(fd);
    std::string nickname = client->get_nickname();
    std::vector<std::string> param = _split_buffer(buffer, SPACE);

    if (buffer.empty())
    {
        _send_response(fd, ERR_NEEDMOREPARAMS(client->get_nickname()));
        _reply_code = 461;
    }
    else if (!client->get_is_logged())
    {
        _send_response(fd, ERR_USERNOTINCHANNEL(nickname, param[0]));
        _reply_code = 451;
    }
    else
    {
        std::vector<std::string> channels = _split_string(buffer, ',');
        for (size_t i = 0; i < channels.size(); ++i) {
            std::string& channel_name = channels[i];
                        
            Channel* channel = _get_channel(channel_name);
            if (!channel)
            {
                _send_response(fd, ERR_NOSUCHCHANNEL(channel_name));
                _reply_code = 403;
            }
            else if (!channel->has_client(client))
            {
                _send_response(fd, ERR_NOTONCHANNEL(channel_name));
                _reply_code = 442;
            }
            else
            {
                channel->part(client);
                _send_response(fd,
                    RPL_PART(client->get_hostname(),
                    channel_name,
                    nickname));
                    _reply_code = 200;
            }
        }
    }
}
