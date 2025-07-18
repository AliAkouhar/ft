#include "Server.hpp"

Server::Server()
{
	_hostname = "ft_irc.42.ma";
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
	_hostname = "ft_irc.42.ma";
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

// void Server::_set_server_socket()
// {
// 	struct pollfd new_poll;
// 	struct sockaddr_in server_addr;

// 	int enable = 1;
// 	server_addr.sin_family = AF_INET;
// 	server_addr.sin_addr.s_addr = INADDR_ANY;
// 	server_addr.sin_port = htons(_port);
// 	_server_fdsocket = socket(AF_INET, SOCK_STREAM, 0);
// 	if (_server_fdsocket == -1)
// 		throw(std::runtime_error("failed to create socket"));
// 	if (setsockopt(_server_fdsocket,
// 				   SOL_SOCKET,
// 				   SO_REUSEADDR,
// 				   &enable,
// 				   sizeof(enable)) == -1)
// 		throw(std::runtime_error(
// 			"failed to set option (SO_REUSEADDR) on socket"));
// 	if (fcntl(_server_fdsocket, F_SETFL, O_NONBLOCK) == -1)
// 		throw(
// 			std::runtime_error("failed to set option (O_NONBLOCK) on socket"));
// 	if (bind(_server_fdsocket,
// 			 (struct sockaddr*)&server_addr,
// 			 sizeof(server_addr)) == -1)
// 		throw(std::runtime_error("failed to bind socket"));
// 	if (listen(_server_fdsocket, SOMAXCONN) == -1)
// 		throw(std::runtime_error("listen() failed"));
// 	new_poll.fd = _server_fdsocket;
// 	new_poll.events = POLLIN;
// 	new_poll.revents = 0;
// 	_fds.push_back(new_poll);
// }

// void Server::init(const std::string& port, const std::string& password)
// {
// 	_is_valid_port(port);

// 	_port = std::atoi(port.c_str());
// 	_password = password;

// 	try
// 	{
// 		_add_server_signal();
// 		_set_server_socket();

// 		std::cout << "Waiting to accept a connection...\n";

// 		_server_loop();
// 		_close_fds();
// 	}
// 	catch (const std::exception& e)
// 	{
// 		_close_fds();
// 		std::cerr << e.what() << '\n';
// 	}
// }

// void Server::_server_loop()
// {
// 	while (Server::_signal == false)
// 	{
// 		if ((poll(&_fds[0], _fds.size(), -1) == -1) && Server::_signal == false)
// 			throw(std::runtime_error("poll() faild"));
// 		for (size_t i = 0; i < _fds.size(); i++)
// 		{
// 			if (_fds[i].revents & POLLIN)
// 			{
// 				if (_fds[i].fd == _server_fdsocket)
// 					_accept_new_client();
// 				else
// 					_receive_new_data(_fds[i].fd);
// 			}
// 		}
// 	}
// }

// void Server::_accept_new_client()
// {
// 	Client cli;
// 	struct pollfd new_poll;
// 	struct sockaddr_in cli_addr;

// 	memset(&cli_addr, 0, sizeof(cli_addr));
// 	socklen_t len = sizeof(cli_addr);
// 	int incofd = accept(_server_fdsocket, (sockaddr*)&(cli_addr), &len);
// 	if (incofd == -1)
// 	{
// 		std::cout << "accept() failed" << std::endl;
// 		return;
// 	}
// 	if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1)
// 	{
// 		std::cout << "fcntl() failed" << std::endl;
// 		close(incofd);
// 		return;
// 	}
// 	new_poll.fd = incofd;
// 	new_poll.events = POLLIN;
// 	new_poll.revents = 0;
// 	cli.set_fd(incofd);
// 	cli.set_ip_add(inet_ntoa((cli_addr.sin_addr)));
// 	_clients.push_back(new Client(cli));
// 	_fds.push_back(new_poll);
// 	std::cout << GRE << "Client <" << incofd << "> Connected" << WHI
// 			  << std::endl;
// }

// void Server::_receive_new_data(const int fd)
// {
// 	char buffer[1024];
// 	std::memset(buffer, 0, sizeof(buffer));

// 	Client* cli = _get_client(fd);
// 	ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
// 	if (bytes <= 0)
// 	{
// 		std::cout << RED << "Client <" << fd << "> Disconnected" << WHI
// 				  << std::endl;
// 		_clear_client(fd);
// 	}
// 	else
// 	{
// 		buffer[bytes] = '\0';
// 		cli->append_to_buffer(buffer);
// 		if (cli->get_buffer().find_first_of(CRLF) != std::string::npos)
// 		{
// 			_execute_command(cli->get_buffer(), fd);
// 			cli->clear_buffer();
// 		}
// 	}
// }

const Server::command_handler Server::_command_list[_command_list_size] = {
	{"JOIN", &Server::_handler_client_join},
	{"QUIT", &Server::_handler_client_quit},
	{"PART", &Server::_handler_client_part},
	{"MODE", &Server::_handler_client_mode},
	{"KICK", &Server::_handler_client_kick},
	{"TOPIC", &Server::_handler_client_topic},
	{"NICK", &Server::_handler_client_nickname},
	{"USER", &Server::_handler_client_username},
	{"PASS", &Server::_handler_client_password},
	{"INVITE", &Server::_handler_client_invite},
	{"PRIVMSG", &Server::_handler_client_privmsg},
};

void Server::_execute_command(const std::string buffer, const int fd)
{
	bool cmd_executed = false;

	if (buffer.empty())
		return;

	std::string clean_buffer = _cleanse_buffer(buffer, CRLF);
	std::vector<std::string> splitted_buffer =
		_split_buffer(clean_buffer, DELIMITER);
	if (splitted_buffer.empty())
		return;
	std::string command = toupper(splitted_buffer[0]);
	std::string parameters = splitted_buffer[1];

	for (size_t i = 0; i < _command_list_size; i++)
	{
		if (command == "WHO")
		{
			cmd_executed = true;
			break;
		}

		if (command == _command_list[i].command)
		{
			(this->*_command_list[i].handler)(parameters, fd);
			cmd_executed = true;
			break;
		}
	}
	if (!cmd_executed)
		_send_response(fd, ERR_CMDNOTFOUND(command));
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

/*
** ----------------------------- CLEAR FUCTIONS -------------------------------
*/

// void Server::_close_fds()
// {
// 	for (size_t i = 0; i < _clients.size(); i++)
// 	{
// 		std::cout << RED << "Client <" << _clients[i]->get_fd()
// 				  << "> Disconnected" << WHI << std::endl;
// 		close(_clients[i]->get_fd());
// 	}
// 	if (_server_fdsocket != -1)
// 	{
// 		std::cout << RED << "Server <" << _server_fdsocket << "> Disconnected"
// 				  << WHI << std::endl;
// 		close(_server_fdsocket);
// 	}
// }

// void Server::_remove_client_from_server(const int fd)
// {
// 	for (std::vector<Client*>::iterator it = _clients.begin();
// 		 it != _clients.end();
// 		 ++it)
// 	{
// 		if ((*it)->get_fd() == fd)
// 		{
// 			delete *it;
// 			*it = NULL;
// 			it = _clients.erase(it);
// 			break;
// 		}
// 	}
// }

// void Server::_remove_client_fd(const int fd)
// {
// 	for (size_t i = 0; i < _fds.size(); i++)
// 	{
// 		if (_fds[i].fd == fd)
// 		{
// 			_fds.erase(_fds.begin() + i);
// 			break;
// 		}
// 	}
// 	close(fd);
// }

// void Server::_remove_client_from_channels(const int fd)
// {
// 	Client* client = _get_client(fd);

// 	for (std::vector<Channel*>::iterator it = _channels.begin();
// 		 it != _channels.end();
// 		 ++it)
// 	{
// 		(*it)->remove_channel_client(client);
// 	}
// }

// void Server::_clear_client(const int fd)
// {
// 	_remove_client_from_channels(fd);
// 	_remove_client_from_server(fd);
// 	_remove_client_fd(fd);
// }

/*
** ----------------------------- SIGNAL FUCTIONS ------------------------------
*/

// bool Server::_signal = false;

// void Server::_signal_handler(const int signum)
// {
// 	(void)signum;
// 	std::cout << std::endl << "Signal Received!" << std::endl;
// 	Server::_signal = true;
// }


// void Server::_add_server_signal()
// {
// 	signal(SIGINT, Server::_signal_handler);
// 	signal(SIGQUIT, Server::_signal_handler);
// }

/*
** --------------------------- VALIDATIONS FUCTIONS ---------------------------
*/

// void Server::_is_valid_port(const std::string& port)
// {
// 	if (!(port.find_first_not_of("0123456789") == std::string::npos &&
// 		  std::atoi(port.c_str()) >= 1024 && std::atoi(port.c_str()) <= 65535))
// 	{
// 		throw std::invalid_argument(
// 			"Invalid port: either contains non-numeric characters or is out of "
// 			"range");
// 	}
// }

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

/*
** ----------------------------- CLIENT FUCTIONS ------------------------------
*/

bool Server::_client_is_ready_to_login(const int fd)
{
	Client* client = _get_client(fd);

	if (!client->get_username().empty() && !client->get_nickname().empty() &&
		!client->get_is_logged())
		return true;
	return false;
}

void Server::_add_channel(Channel* channel) { _channels.push_back(channel); }

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
