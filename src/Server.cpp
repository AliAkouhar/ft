#include "Server.hpp"

Server::Server()
{
	_hostname = "ft_irc.ma";
	_reply_code = 0;
	_port = 0;
	_server_fdsocket = -1;
	_password = "";
	memset(&_server_addr, 0, sizeof(_server_addr));
	_fds = std::vector<struct pollfd>();
	_clients = std::vector<Client*>();
	_channels = std::vector<Channel*>();
}

Server::Server(std::string password, std::vector<Client*> clients,
			   std::vector<Channel*> channels)
{
	_hostname = "ft_irc.ma";
	_reply_code = 0;
	_port = 0;
	_server_fdsocket = -1;
	_password = password;
	memset(&_server_addr, 0, sizeof(_server_addr));
	_fds = std::vector<struct pollfd>();
	this->_clients = clients;
	this->_channels = channels;
}


Server::~Server()
{
	for (std::vector<Client*>::iterator it = _clients.begin();
		 it != _clients.end();
		 ++it)
	{
		delete *it;
	}
	_clients.clear();

	for (std::vector<Channel*>::iterator it = _channels.begin();
		 it != _channels.end();
		 ++it)
	{
		delete *it;
	}
	_channels.clear();
}

std::string Server::_get_hostname() { return _hostname; }

Client* Server::_get_client(const int fd)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i]->get_fd() == fd)
		{
			return _clients[i];
		}
	}
	return NULL;
}

Client* Server::_get_client(const std::string nickname)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i]->get_nickname() == nickname)
		{
			return _clients[i];
		}
	}
	return NULL;
}

Channel* Server::_get_channel(const std::string& channel_name)
{
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i]->get_name() == channel_name)
		{
			return _channels[i];
		}
	}
	return NULL;
}

void Server::_send_response(const int fd, const std::string& response)
{
	std::cout << "Response:\n" << response;
	if (send(fd, response.c_str(), response.size(), 0) == -1)
		std::cerr << "Response send() failed" << std::endl;
}

std::string toupper(const std::string& str)
{
	std::string result = str;
	for (size_t i = 0; i < result.length(); ++i)
	{
		result[i] = std::toupper(result[i]);
	}
	return result;
}

std::string Server::_cleanse_buffer(const std::string& buffer,
									const std::string& chars_to_remove)
{
	std::string clean_buffer;

	size_t pos = buffer.find_first_of(chars_to_remove);
	if (pos != std::string::npos)
		clean_buffer = buffer.substr(0, pos);
	else
		clean_buffer = buffer;
	return clean_buffer;
}

std::vector<std::string> _split_commd(const std::string& buffer,
											   const std::string& delimiter)
{
	std::vector<std::string> tokens;
    std::string::size_type prev_pos = 0;
    std::string::size_type pos = buffer.find(delimiter);

    if (pos != std::string::npos) {
        tokens.push_back(buffer.substr(prev_pos, pos - prev_pos));
        prev_pos = pos + 1;
    }
    tokens.push_back(buffer.substr(prev_pos, std::string::npos)); // Add the last token
    return tokens;
}

std::vector<std::string> _split_string(const std::string& s, char delimiter) {
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

// const Server::command_handler Server::_command_list[_command_list_size] = {
// 	{"JOIN", &Server::_handler_client_join},
// 	{"QUIT", &Server::_handler_client_quit},
// 	{"PART", &Server::_handler_client_part},
// 	{"MODE", &Server::_handler_client_mode},
// 	{"KICK", &Server::_handler_client_kick},
// 	{"TOPIC", &Server::_handler_client_topic},
// 	{"NICK", &Server::_handler_client_nickname},
// 	{"USER", &Server::_handler_client_username},
// 	{"PASS", &Server::_handler_client_password},
// 	{"INVITE", &Server::_handler_client_invite},
// 	{"PRIVMSG", &Server::_handler_client_privmsg}
// };

std::map<std::string, void (Server::*)(const std::string &, int)> 

command_map = {
	{"PART", &Server::_handler_client_part},
    {"JOIN", &Server::_handler_client_join},
    {"QUIT", &Server::_handler_client_quit},
	{"MODE", &Server::_handler_client_mode},
	{"KICK", &Server::_handler_client_kick},
	{"TOPIC", &Server::_handler_client_topic},
	{"NICK", &Server::_handler_client_nickname},
	{"USER", &Server::_handler_client_username},
	{"PASS", &Server::_handler_client_password},
	{"INVITE", &Server::_handler_client_invite},
	{"PRIVMSG", &Server::_handler_client_privmsg}
};

void Server::_execute_command(const std::string buffer, const int fd)
{
	bool cmd_executed = false;

	if (buffer.empty())
		return;

	std::string clean_buffer = _cleanse_buffer(buffer, CRLF);
	std::vector<std::string> splitted_buffer =
		_split_commd(clean_buffer, DELIMITER);
	if (splitted_buffer.empty())
		return;
	std::string command = toupper(splitted_buffer[0]);
	std::string parameters = splitted_buffer[1];
 
	// for (size_t i = 0; i < _command_list_size; i++)
	// {
		if (command == "WHO")
		{
			cmd_executed = true;
			// break;
		}
		else if (command_map.count(command))
		{
			(this->*command_map[command])(parameters, fd);
			cmd_executed = true;
			// break;
		}

		// if (command == _command_list[i].command)
		// {
		// 	(this->*_command_list[i].handler)(parameters, fd);
		// 	cmd_executed = true;
		// 	break;
		// }
	// }
	if (!cmd_executed)
		_send_response(fd, ERR_CMDNOTFOUND(command));
}
