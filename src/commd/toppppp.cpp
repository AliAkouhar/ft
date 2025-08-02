
void Server::_ft_topic(const std::string& buffer, const int fd) {
    Client* client = _get_client(fd);

    // Split parameters: <channel> [<topic>]
    std::vector<std::string> params = _split_buffer(buffer, SPACE);
 
    std::string channel_name = params[0];
    Channel* channel = _get_channel(channel_name);

    // Run standard checks (user in channel, channel exists, etc.)
    if (_topic_checks(client, fd, channel_name, params))
        return;

    // ---------- 1. Query Topic ----------
    if (params.size() == 1) {
        std::string topic = channel->get_topic();
        if (topic.empty()) {
            _send_response(fd, RPL_NOTOPIC(client->get_nickname(), channel_name));
            _reply_code = 331;  // No topic
        } else {
            _send_response(fd, RPL_TOPIC(client->get_nickname(), channel_name, topic));
            _reply_code = 332;  // Topic exists
        }
        return;
    }

    // ---------- 2. Change Topic ----------
    // Check +t restriction
    if (channel->get_topic_restriction() && 
        !channel->is_channel_operator(client->get_nickname())) {
        _send_response(fd, ERR_CHANOPRIVSNEEDED(channel_name));
        _reply_code = 482;
        return;
    }

    // Extract everything after the first space following the channel name
    size_t pos = buffer.find(channel_name);
    pos = buffer.find(' ', pos);  // Find space after channel name
    if (pos == std::string::npos) return; // Safety check

    std::string new_topic = buffer.substr(pos + 1);

    // Remove leading ':' if present
    if (!new_topic.empty() && new_topic[0] == ':')
        new_topic.erase(0, 1);

    channel->set_topic(new_topic);
    _send_response(fd, RPL_TOPIC(client->get_nickname(), channel_name, new_topic));
    _reply_code = 332;
}
