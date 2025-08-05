#include "../inc/Server.hpp"

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

void Server::_remove_client_from_server(const int fd)
{
	for (std::vector<Client*>::iterator it = _clients.begin();
		 it != _clients.end();
		 ++it)
	{
		if ((*it)->get_fd() == fd)
		{
        	delete *it;
			*it = NULL;
			it = _clients.erase(it);
			break;
		}
	}
}

void Server::_remove_client_from_channels(const int fd)
{
	Client* client = _get_client(fd);

	for (std::vector<Channel*>::iterator it = _channels.begin();
		 it != _channels.end();
		 ++it)
	{
		(*it)->remove_channel_client(client);
	}
}

void Server::_remove_client_fd(const int fd)
{
	for (size_t i = 0; i < _fds.size(); i++)
	{
		if (_fds[i].data.fd == fd)
		{
			_fds.erase(_fds.begin() + i);
			break;
		}
	}
	close(fd);
}

void Server::_clear_client(const int fd)
{
	_remove_client_from_channels(fd);
	/* _remove_client_from_server(fd); */
	_remove_client_fd(fd);
}

bool Server::_client_is_ready_to_login(const int fd)
{
	Client* client = _get_client(fd);

	if (!client->get_username().empty() && !client->get_nickname().empty() &&
		!client->get_is_logged())
		return true;
	return false;
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
		std::cerr << "Responsed sending failed" << std::endl;
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

std::string Server::_remove_rn(const std::string& buffer,
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

bool Server::_is_nickname_in_use(const int fd, const std::string& username)
{
	for (std::vector<Client*>::iterator it = _clients.begin();
		 it != _clients.end();
		 ++it)
	{
		if ((*it)->get_nickname() == username && (*it)->get_fd() != fd)
			return true;
	}
	return false;
}

bool Server::_is_valid_nickname(const std::string& nickname)
{
	if (nickname.size() <= 3)
		return false;
	for (std::string::const_iterator it = nickname.begin();
		 it != nickname.end();
		 ++it)
	{
		if (!std::isalnum(*it))
			return false;
	}
	return true;
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
    tokens.push_back(buffer.substr(prev_pos, std::string::npos));
    return tokens;
}

std::vector<std::string> Server::_split_buffer(const std::string& buffer,
                                               const std::string& delimiter)
{
    std::string command;
    std::string parameters;
    std::vector<std::string> tokens;
    std::istringstream iss(buffer);

	iss >> command;
	tokens.push_back(command);

	std::getline(iss >> std::ws, parameters);
	parameters.erase(0, parameters.find_first_not_of(delimiter));
	tokens.push_back(parameters);

	return tokens;
}

void Server::_handle_commands(const std::string& command, const std::string& parameters, const int fd)
{
    if (command == "PART")
        _ft_part(parameters, fd);
    else if (command == "JOIN")
        _ft_join(parameters, fd);
    else if (command == "QUIT")
        _ft_quit(parameters, fd);
    else if (command == "MODE")
        _ft_mode(parameters, fd);
    else if (command == "KICK")
        _ft_kick(parameters, fd);
    else if (command == "TOPIC")
        _ft_topic(parameters, fd);
    else if (command == "NICK")
        _ft_nickname(parameters, fd);
    else if (command == "USER")
        _ft_username(parameters, fd);
    else if (command == "PASS")
        _ft_password(parameters, fd);
    else if (command == "INVITE")
        _ft_invite(parameters, fd);
    else if (command == "PRIVMSG")
        _ft_privmsg(parameters, fd);
    else if (command == "CAP")
		return ;
	else
        _send_response(fd, ERR_CMDNOTFOUND(command));
}

void Server::_exec_cmd(const std::string buffer, const int fd)
{
	if (buffer.empty()) 
		return;

	std::string parameters;
	std::string clean_buffer = _remove_rn(buffer, CRLF);
	std::vector<std::string> splitted_buffer =
		_split_commd(buffer, SPACE);

	if (splitted_buffer.empty())
		return ;

	std::string command = toupper(splitted_buffer[0]);
    if (splitted_buffer[1].empty())
        parameters = "";
	else if (splitted_buffer.size() > 1)
    	parameters = splitted_buffer[1];

	_handle_commands(command, parameters, fd);
}

void Server::_add_channel(Channel* channel) { _channels.push_back(channel); }
