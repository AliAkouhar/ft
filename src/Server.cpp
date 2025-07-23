#include "../inc/Server.hpp"


#define MAX_EVENTS 20
#define MAX_BUFFER_SIZE 4096
// Server::Server()
// {
// 	_hostname = "ft_irc.ma";
// 	_reply_code = 0;
// 	_port = 1337;
// 	_server_fdsocket = -1;
// 	_password = "";
// 	memset(&_server_addr, 0, sizeof(_server_addr));
// 	// _fds = std::vector<struct pollfd>();
// 	_clients = std::vector<Client*>();
// 	_channels = std::vector<Channel*>();
// }

// Server::Server(std::string password, std::vector<Client*> clients,
// 			   std::vector<Channel*> channels)
// {
// 	_hostname = "ft_irc.ma";
// 	_reply_code = 0;
// 	_port = 1337;
// 	_server_fdsocket = -1;
// 	_password = password;
// 	memset(&_server_addr, 0, sizeof(_server_addr));
// 	// _fds = std::vector<struct pollfd>();
// 	this->_clients = clients;
// 	this->_channels = channels;
// }
//Ali



// Server::~Server()
// {
// 	for (std::vector<Client*>::iterator it = _clients.begin();
// 		 it != _clients.end();
// 		 ++it)
// 	{
// 		delete *it;
// 	}
// 	_clients.clear();

// 	for (std::vector<Channel*>::iterator it = _channels.begin();
// 		 it != _channels.end();
// 		 ++it)
// 	{
// 		delete *it;
// 	}
// 	_channels.clear();
// }
//ALI

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

std::string Server::_get_hostname() { return _hostname; }

// Client* Server::_get_client(const int fd)
// {
// 	for (size_t i = 0; i < _clients.size(); i++)
// 	{
// 		if (_clients[i]->get_fd() == fd)
// 		{
// 			return _clients[i];
// 		}
// 	}
// 	return NULL;
// }

// Client* Server::_get_client(const std::string nickname)
// {
// 	for (size_t i = 0; i < _clients.size(); i++)
// 	{
// 		if (_clients[i]->get_nickname() == nickname)
// 		{
// 			return _clients[i];
// 		}
// 	}
// 	return NULL;
// }

// Channel* Server::_get_channel(const std::string& channel_name)
// {
// 	for (size_t i = 0; i < _channels.size(); i++)
// 	{
// 		if (_channels[i]->get_name() == channel_name)
// 		{
// 			return _channels[i];
// 		}
// 	}
// 	return NULL;
// }

// void Server::_send_response(const int fd, const std::string& response)
// {
// 	std::cout << "Response:\n" << response;
// 	if (send(fd, response.c_str(), response.size(), 0) == -1)
// 		std::cerr << "Response send() failed" << std::endl;
// }

// std::string toupper(const std::string& str)
// {
// 	std::string result = str;
// 	for (size_t i = 0; i < result.length(); ++i)
// 	{
// 		result[i] = std::toupper(result[i]);
// 	}
// 	return result;
// }

// std::string Server::_cleanse_buffer(const std::string& buffer,
// 									const std::string& chars_to_remove)
// {
// 	std::string clean_buffer;

// 	size_t pos = buffer.find_first_of(chars_to_remove);
// 	if (pos != std::string::npos)
// 		clean_buffer = buffer.substr(0, pos);
// 	else
// 		clean_buffer = buffer;
// 	return clean_buffer;
// }

// std::vector<std::string> _split_commd(const std::string& buffer,
// 											   const std::string& delimiter)
// {
// 	std::vector<std::string> tokens;
//     std::string::size_type prev_pos = 0;
//     std::string::size_type pos = buffer.find(delimiter);

//     if (pos != std::string::npos) {
//         tokens.push_back(buffer.substr(prev_pos, pos - prev_pos));
//         prev_pos = pos + 1;
//     }
//     tokens.push_back(buffer.substr(prev_pos, std::string::npos)); // Add the last token
//     return tokens;
// }

// std::vector<std::string> _split_string(const std::string& s, char delimiter) {
//     std::vector<std::string> tokens;
//     size_t start = 0;
//     size_t end = s.find(delimiter);

//     while (end != std::string::npos) {
//         tokens.push_back(s.substr(start, end - start));
//         start = end + 1;
//         end = s.find(delimiter, start);
//     }
//     tokens.push_back(s.substr(start, std::string::npos));
//     return tokens;
// }

// std::map<std::string, void (Server::*)(const std::string &, int)> 

// command_map = {
// 	{"PART", &Server::_handler_client_part},
//     {"JOIN", &Server::_handler_client_join},
//     {"QUIT", &Server::_handler_client_quit},
// 	{"MODE", &Server::_handler_client_mode},
// 	{"KICK", &Server::_handler_client_kick},
// 	{"TOPIC", &Server::_handler_client_topic},
// 	{"NICK", &Server::_handler_client_nickname},
// 	{"USER", &Server::_handler_client_username},
// 	{"PASS", &Server::_handler_client_password},
// 	{"INVITE", &Server::_handler_client_invite},
// 	{"PRIVMSG", &Server::_handler_client_privmsg}
// };

// void Server::_execute_command(const std::string buffer, const int fd)
// {
// 	bool cmd_executed = false;

// 	if (buffer.empty())
// 		return;

// 	std::string clean_buffer = _cleanse_buffer(buffer, CRLF);
// 	std::vector<std::string> splitted_buffer =
// 		_split_commd(clean_buffer, DELIMITER);

// 	if (splitted_buffer.empty())
// 		return ;

// 	std::string command = toupper(splitted_buffer[0]);
// 	std::string parameters = splitted_buffer[1];
	
// 	if (command_map.count(command))
// 	{
// 		(this->*command_map[command])(parameters, fd);
// 		cmd_executed = true;
// 	}
// 	if (!cmd_executed)
// 		_send_response(fd, ERR_CMDNOTFOUND(command));
// }
// SERVER METHODS

Server::Server() : _port(7071), _server_fdsocket(-1){}

Server::Server(char **av)
{
    // std::cout << "find" <<av[1] <<  std::endl;
    _port = atoi(av[1]);
    _pass = av[2];
    _ip = "127.0.0.1";
    _server_fdsocket = -1;
    memset(&_server_addr, 0, sizeof(_server_addr));
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_addr.s_addr = INADDR_ANY;
    _server_addr.sin_port = htons(_port);
}

Server::~Server() {}

void Server::createSocket()
{
    _server_fdsocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fdsocket < 0)
    {
        std::cerr << "socket" << std::endl;
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
        std::cerr << "bind" << std::endl;
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

void Server::acceptClient()
{
    std::cout << "here"     << std::endl;
    struct sockaddr_in clientAddr;
    socklen_t addrlen = sizeof(clientAddr);
    int clientFd = accept(_server_fdsocket, (struct sockaddr *)&clientAddr, &addrlen);
    if (clientFd < 0)
    {
        std::cerr << "Errir when accepting client" << std::endl;
        exit(1);
    }
    int res = makeSocketNonBlocking(clientFd);
    if(res < 0)
    {
        std::cerr << "Failed to make client socket non-blocking" << std::endl;
        closeSocket(clientFd);
        exit(1);
    }
    struct epoll_event event;
    event.data.fd = clientFd;
    event.events = EPOLLIN;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &event) < 0)
    {
        std::cerr << "Failed to add client socket to epoll" << std::endl;
        closeSocket(clientFd);
        exit(1);
    }
}

void Server::closeSocket(int socketFd)
{
    close(socketFd);
}

void Server::readSocket()
{
    while (1)
    {
        struct epoll_event events[MAX_EVENTS];
        int nfds = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (nfds < 0)
        {
            std::cerr << "Error in epoll_wait" << std::endl;
            exit(1);
        }
        for (int i = 0; i < nfds; i++)
        {
            if (events[i].data.fd == _server_fdsocket)
                acceptClient();
            else
            {
                char buffer[MAX_BUFFER_SIZE];
                if (events[i].events & EPOLLIN)
                {
                    int totalBytesRecieved = 0;
                    int bytesRecv = recv(events[i].data.fd, buffer + totalBytesRecieved, MAX_BUFFER_SIZE - totalBytesRecieved, 0);
                    if (bytesRecv == 0)
                    {
                        std::cout << "Client disconnected" << std::endl;
                        closeSocket(events[i].data.fd);
                        epoll_ctl(epollFd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                        break;
                    }
                    else if (bytesRecv < 0)
                    {
                        std::cerr << "Error when receiving data" << std::endl;
                        closeSocket(events[i].data.fd);
                        epoll_ctl(epollFd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                        break;
                    }
                    else
                    {
                        totalBytesRecieved += bytesRecv;
                        if (bytesRecv < MAX_BUFFER_SIZE)
                        {
                            buffer[totalBytesRecieved] = '\0';
                            std::cout << "buffer : " << buffer << std::endl;
                            //you can call your parser function here
                        }
                        else
                        {
                            std::cerr << "Buffer overflow detected, closing connection" << std::endl;
                        }
                        // buffer + '\0'; + fd of the client that triger the event
                    }
                }
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