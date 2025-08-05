#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"

/*
 * Parameters: <channel> [<topic>]
 * Reference: https://datatracker.ietf.org/doc/html/rfc1459#section-4.2.4
 */

void Server::_ft_topic(const std::string& buffer, const int fd) {
    std::vector<std::string> params = _split_buffer(buffer, SPACE);
    if (params.empty()) return;
    Client* client = _get_client(fd);

    std::string channel_name = params[0];
    Channel* channel = _get_channel(channel_name);
    if (_topic_checks(client, fd, channel_name, params)) {
        return;
    }

    if (params[1].empty())
    {
        std::string topic = channel->get_topic();
        if (topic.empty())
        {
            _send_response(fd, RPL_NOTOPIC(client->get_nickname(), channel_name));
            _reply_code = 331;
        }
        else
        {
            _send_response(fd, RPL_TOPIC(client->get_nickname(), channel_name, topic));
            _reply_code = 332;
        }
        return;
    }

    if (channel->get_topic_restriction() &&
        !channel->is_channel_operator(client->get_nickname()))
    {
        _send_response(fd, ERR_CHANOPRIVSNEEDED(channel_name));
        _reply_code = 482;
        return;
    }

    size_t pos = buffer.find(channel_name);
    if (pos != std::string::npos)
    {
        pos += channel_name.length() + 1;
        std::string new_topic = buffer.substr(pos);
        
        channel->set_topic(new_topic);
        _send_response(fd, RPL_TOPIC(client->get_nickname(), channel_name, new_topic));
        _reply_code = 332;
    }
}
