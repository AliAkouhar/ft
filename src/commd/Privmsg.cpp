#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"
#include "../../inc/Client.hpp"
#include "../../inc/Replies.hpp"

std::vector<std::string> split_parameters(const std::string& s, const std::string& delimiter) {
	std::vector<std::string> tokens;
	size_t pos = 0, next;
	while ((next = s.find(delimiter, pos)) != std::string::npos) {
		tokens.push_back(s.substr(pos, next - pos));
		pos = next + delimiter.length();
	}
	tokens.push_back(s.substr(pos));
	return tokens;
}

void Server::_ft_privmsg(const std::string& buffer, const int fd) {
	std::vector<std::string> params = _split_buffer(buffer, " ");
	Client* client = _get_client(fd);
	
	std::vector<std::string> receivers = split_parameters(params[0], ",");
	if (_privmsg_checks(client, receivers))
	{
		return ;
	}
	
	const std::string& message = params[1];
	for (std::vector<std::string>::iterator it = receivers.begin(); it != receivers.end(); ++it) {
		const std::string& target = *it;

		if (target[0] == '#') {
			Channel* channel = _get_channel(target);
			channel->broadcast(client, channel->get_name(), message);
		}
		else
		{
			Client* target_client = _get_client(target);
			_send_response(target_client->get_fd(), RPL_PRIVMSG(
				client->get_nickname(),
				client->get_hostname(),
				target_client->get_nickname(),
				message));
		}
	}
	_reply_code = 200;
}
