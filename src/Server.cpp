#include "../inc/Server.hpp"
#include "../inc/Channel.hpp"
#include "../inc/Client.hpp"
#include "../inc/Replies.hpp"

#define MAX_EVENTS 20
#define MAX_BUFFER_SIZE 4096

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
//ALI

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
	// _remove_client_from_server(fd);
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

// Ctrl+V, then Ctrl+M for \r, and Ctrl+V, Ctrl+J for \n

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
    else
        _send_response(fd, ERR_CMDNOTFOUND(command));
}

void Server::_exec_cmd(const std::string buffer, const int fd)
{
	if (buffer.empty()) 
		return;

	std::string clean_buffer = _remove_rn(buffer, CRLF);
	std::vector<std::string> splitted_buffer =
		_split_commd(buffer, SPACE);

	if (splitted_buffer.empty())
		return ;

	std::string command = toupper(splitted_buffer[0]);
    if (splitted_buffer[1].empty())
        return ;
    std::string parameters = splitted_buffer[1];

	_handle_commands(command, parameters, fd);
}

// SERVER METHODS

void Server::_add_channel(Channel* channel) { _channels.push_back(channel); }

Server::Server() : _port(7071), _server_fdsocket(-1){}

Server::Server(char **av)
{
    _fds.resize(MAX_EVENTS); // Initialize the epoll events vector
    _port = atoi(av[1]);
    _pass = av[2];
    _ip = "127.0.0.1";
    _server_fdsocket = -1;
    memset(&_server_addr, 0, sizeof(_server_addr));
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_addr.s_addr = INADDR_ANY;
    _server_addr.sin_port = htons(_port);
}

void Server::createSocket()
{
    _server_fdsocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fdsocket < 0)
    {
        std::cerr << "socket" << std::endl;
        exit(1);
    }
    
    // Enable socket address reuse to avoid "Address already in use" errors
    int opt = 1;
    if (setsockopt(_server_fdsocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        std::cerr << "setsockopt SO_REUSEADDR failed" << std::endl;
        exit(1);
    }
}

void Server::creatEpoll()
{
    epollFd = epoll_create1(0);
    if (epollFd < 0)
    {
        std::cerr << "Failed to create epoll instance" << std::endl;
        exit(1);
    }
}

void Server::bindSocket()
{
    int bindRes = bind(_server_fdsocket, (struct sockaddr *)&_server_addr, sizeof(_server_addr));
    if (bindRes < 0)
    {
        std::cerr << "Cant bind IP Address" << std::endl;
        exit(1);
    }
}

void Server::listenSocket()
{
    int lstn = listen(_server_fdsocket, SOMAXCONN);
    if (lstn < 0)
    {
        std::cerr << "Failed to listen on socket" << std::endl;
        exit(1);
    }
}

void Server::addSocketToEpoll()
{
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = _server_fdsocket;
    
    int res = epoll_ctl(epollFd, EPOLL_CTL_ADD, _server_fdsocket, &ev);
    if (res < 0)
    {
        std::cerr << "Failed to add socket to epoll" << std::endl;
        exit(1);
    }
}

static int makeSocketNonBlocking(int sfd)
{
    int flags, s;

    flags = fcntl(sfd, F_GETFL, 0);
    if(flags == -1)
    {
        std::cerr << "fcntl Error" << std::endl;
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl(sfd, F_SETFL, flags);
    if(s == -1)
    {
        std::cerr << "fcntl Error" << std::endl;
        return -1;
    }
    return 0;
}

void Server::closeSocket(int socketFd)
{
    close(socketFd);
}

void Server::acceptClient()
{
    struct sockaddr_in clientAddr;
    socklen_t addrlen = sizeof(clientAddr);
    int clientFd = accept(_server_fdsocket, (struct sockaddr *)&clientAddr, &addrlen);
    if (clientFd < 0)
    {
        std::cerr << "Error when accepting client" << std::endl;
        exit(1);
    }
    int res = makeSocketNonBlocking(clientFd);
    if(res < 0)
    {
        std::cerr << "Failed to make client socket non-blocking" << std::endl;
        closeSocket(clientFd);
        exit(1);
    }
    
    // Create new Client object
    Client* newClient = new Client();
    newClient->set_fd(clientFd);
    
    // Set client IP address
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
    newClient->set_ip_add(std::string(clientIP));
    
    // Add client to our clients vector
    _clients.push_back(newClient);
    
    struct epoll_event event;
    event.data.fd = clientFd;
    event.events = EPOLLIN;
    
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &event) < 0)
    {
        std::cerr << "Failed to add client socket to epoll" << std::endl;
        delete newClient;
        _clients.pop_back();
        closeSocket(clientFd);
        exit(1);
    }
    
    std::cout << "New client connected: " << clientIP << " (fd: " << clientFd << ")" << std::endl;
}


void Server::handleClientData(int clientFd)
{
    char buffer[MAX_BUFFER_SIZE];
    Client* client = _get_client(clientFd);
    if (!client)
    {
        std::cerr << "Client not found for fd: " << clientFd << std::endl;
        closeSocket(clientFd);
        epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, NULL);
        return;
    }
    
    int bytesRecv = recv(clientFd, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (bytesRecv == 0)
    {
        std::cout << "Client disconnected (fd: " << clientFd << ")" << std::endl;
        // Remove client from vector
        for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            if ((*it)->get_fd() == clientFd)
            {
                delete *it;
                _clients.erase(it);
                break;
            }
        }
        closeSocket(clientFd);
        epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, NULL);
        return;
    }
    else if (bytesRecv < 0)
    {
        std::cerr << "Error when receiving data from fd: " << clientFd << std::endl;
        // Remove client from vector
        for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            if ((*it)->get_fd() == clientFd)
            {
                delete *it;
                _clients.erase(it);
                break;
            }
        }
        closeSocket(clientFd);
        epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, NULL);
        return;
    }
    else
    {
        // Null-terminate the received data
        buffer[bytesRecv] = '\0';
        client->append_to_buffer(std::string(buffer));        
        std::string clientBuffer = client->get_buffer();
        std::string::size_type pos = 0;
        
        while ((pos = clientBuffer.find("\r\n")) != std::string::npos)
        {
            std::string completeMessage = clientBuffer.substr(0, pos);
            
            // Remove the complete message from the buffer            
            clientBuffer = clientBuffer.substr(pos + 2);
            
            if (!completeMessage.empty())
            {
                // std::cout << "=== COMPLETE MESSAGE RECEIVED ===" << std::endl;
                _exec_cmd(completeMessage.c_str(), clientFd);
            }
        }
        
        // Update client buffer with remaining partial data
        client->set_buffer(clientBuffer);
        
        // Show partial buffer information if there's remaining data
        if (!clientBuffer.empty())
        {
            std::cout << "--- PARTIAL BUFFER ---" << std::endl;
            std::cout << "Client FD: " << clientFd << std::endl;
            std::cout << "Client IP: " << client->get_ip_address() << std::endl;
            std::cout << "Partial Buffer: '" << clientBuffer << "'" << std::endl;
            std::cout << "Buffer Length: " << clientBuffer.length() << " bytes" << std::endl;
            std::cout << "Waiting for \\r\\n delimiter..." << std::endl;
            std::cout << "----------------------" << std::endl;
        }
    }
}

void Server::readSocket()
{
    while (1)
    {
        // Use the _fds vector instead of local array
        int nfds = epoll_wait(epollFd, _fds.data(), MAX_EVENTS, -1);
        if (nfds < 0)
        {
            std::cerr << "Error in epoll_wait" << std::endl;
            exit(1);
        }
        for (int i = 0; i < nfds; i++)
        {
            if (_fds[i].data.fd == _server_fdsocket)
            {
                // Handle new client connection
                acceptClient();
            }
            else if (_fds[i].events & EPOLLIN)
            {
                // Handle existing client data
                handleClientData(_fds[i].data.fd);
            }
        }
    }
}

void Server::setup()
{
   this->createSocket();
   this->creatEpoll();
   this->bindSocket();
   this->listenSocket();
   this->addSocketToEpoll();
   this->readSocket();
}