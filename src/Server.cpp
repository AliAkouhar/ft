#include "../inc/Server.hpp"
#include "../inc/Channel.hpp"
#include "../inc/Client.hpp"
#include "../inc/Replies.hpp"

#define MAX_EVENTS 20
#define MAX_BUFFER_SIZE 4096

//new server

Server* g_server = NULL;
bool g_signal_received = false;

// Signal handler function
void signalHandler(int signal)
{
    std::cout << "\nSignal " << signal << " received. Shutting down server..." << std::endl;
    g_signal_received = true;
    if (g_server)
        g_server->shutdown();
}

Server::Server()
{
    _port = 0;
    _server_fdsocket = -1;
    epollFd = -1;
    _hostname = "localhost";
    _pass = "default_password";
    _ip = "127.0.0.1";
    g_server = this; // Set global pointer for signal handling
}



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

Server::~Server()
{
    // Clean up all client objects
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        delete _clients[i];
    }
    _clients.clear();

    // Clean up all channel objects
    for (size_t i = 0; i < _channels.size(); ++i)
    {
        delete _channels[i];
    }
    _channels.clear();

    if (_server_fdsocket != -1)
        close(_server_fdsocket);
    if (epollFd != -1)
        close(epollFd);
}
    
    Server::Server(char **av)
    {
        _port = _get_port(av[1]);
        _pass = av[2];
        _server_fdsocket = -1;
        epollFd = -1;
        _hostname = "localhost";
        _ip = "127.0.0.1";
        g_server = this; // Set global pointer for signal handling
    }
    void Server::createSocket()
    {
        _server_fdsocket = socket(AF_INET, SOCK_STREAM, 0);
        if (_server_fdsocket < 0)
        throw std::runtime_error("Failed to create socket\n");
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
    


int Server::_get_port(std::string port)
{
    if (port.find_first_not_of("0123456789") != std::string::npos)
        throw std::runtime_error("Invalid port number\n");
    int portNum = atoi(port.c_str());
    if (portNum < 1024 || portNum > 65535)
        throw std::runtime_error("Port number must be between 1024 and 65535\n");
    return portNum;
}

// void Server::createSocket()
// {
//     _server_fdsocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (_server_fdsocket < 0)
//         throw std::runtime_error("Failed to create socket\n");
// }

void Server::setSocketReused()
{
    int opt = 1;
    if (setsockopt(_server_fdsocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("Failed to set socket options for reuse\n");
}

void Server::setSocketNonBlocking()
{
    int flags = fcntl(_server_fdsocket, F_GETFL, 0);
    if (flags == -1)
        throw std::runtime_error("Failed to get socket flags\n");
    if (fcntl(_server_fdsocket, F_SETFL, flags | O_NONBLOCK) == -1)
        throw std::runtime_error("Failed to set socket to non-blocking\n");
}


void Server::bindSocket()
{
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_addr.s_addr = inet_addr(_ip.c_str());
    _server_addr.sin_port = htons(_port);

    if (bind(_server_fdsocket, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0)
        throw std::runtime_error("Failed to bind socket\n");
}

void Server::listenSocket()
{
    if (listen(_server_fdsocket, SOMAXCONN) < 0)
        throw std::runtime_error("Failed to listen on socket\n");
}

void Server::addSocketToEpoll()
{
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = _server_fdsocket;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, _server_fdsocket, &ev) < 0)
        throw std::runtime_error("Failed to add socket to epoll\n");
}

void Server::creatEpoll()
{
    epollFd = epoll_create1(0);
    if (epollFd < 0)
        throw std::runtime_error("Failed to create epoll instance\n");
}

void Server::closeSocket(int socketFd)
{
    if (close(socketFd) < 0)
        throw std::runtime_error("Failed to close socket\n");
}

void Server::acceptClient()
{
    Client* new_client = new Client();
    struct sockaddr_in clientAddr;
    socklen_t addrlen = sizeof(clientAddr);
    int client_fd = accept(_server_fdsocket, (struct sockaddr *)&clientAddr, &addrlen);
    if (client_fd < 0)
        throw std::runtime_error("Error when accepting client\n");
    int res = makeSocketNonBlocking(client_fd);
    if (res < 0)
    {
        closeSocket(client_fd);
        delete new_client;
        throw std::runtime_error("Failed to make client socket non-blocking\n");
    }
    new_client->set_fd(client_fd);
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
    new_client->set_ip_add(std::string(clientIP));
    _clients.push_back(new_client);
    struct epoll_event event;
    event.data.fd = client_fd;
    event.events = EPOLLIN;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, client_fd, &event) < 0)
    {
        _clients.pop_back();
        closeSocket(client_fd);
        delete new_client;
        throw std::runtime_error("Failed to add client socket to epoll\n");
    }
    std::cout << "New client connected: " << clientIP << " (fd: " << client_fd << ")" << std::endl;
}

void Server::handleClientData(int clientFd)
{
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    int readBytes = recv(clientFd, buffer, sizeof(buffer) - 1, 0);
    if (readBytes <= 0)
    {
        // Find and remove client
        for (size_t i = 0; i < _clients.size(); ++i)
        {
            if (_clients[i]->get_fd() == clientFd)
            {
                // Remove from epoll
                epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, NULL);
                // Close socket
                closeSocket(clientFd);
                // Delete client object and remove from vector
                delete _clients[i];
                _clients.erase(_clients.begin() + i);
                std::cout << "Client disconnected: fd " << clientFd << std::endl;
                break;
            }
        }
    }
    else
    {
        buffer[readBytes] = '\0';
        for (size_t i = 0; i < _clients.size(); i++)
        {
            if (_clients[i]->get_fd() == clientFd)
            {
                std::string clientBuff = _clients[i]->get_buffer();
                clientBuff += buffer;
                _clients[i]->set_buffer(clientBuff);
                size_t pos = 0; // i must understand it
                while ((pos = clientBuff.find("\r\n")) != std::string::npos)
                {
                    std::string completMessage = clientBuff.substr(0, pos);
                    clientBuff.erase(0, pos + 2); // remove the c
                    // Process the complete message hereomplete message from the
                    _clients[i]->set_buffer(clientBuff);
                    std::cout << "complete message received: " << completMessage << std::endl;
                    Server::_exec_cmd(completMessage, _clients[i]->get_fd());
                    // Process the complete message here
                    // continue; // or continue to process more messages
                }
                // break;
            }
        }
    }
}

void Server::readSocket()
{
    // const int MAX_EVENTS = 64;
    struct epoll_event events[MAX_EVENTS];
    std::cout << "Server is running at port " << _port << " and waiting for connections..." << std::endl;
    
    while (!g_signal_received)
    {
        int nfds = epoll_wait(epollFd, events, MAX_EVENTS, 1000); // 1 second timeout
        if (nfds < 0)
        {
            // epoll_wait failed, but continue if it's due to signal interruption
            continue;
        }
        
        for (int i = 0; i < nfds; i++)
        {
            if (events[i].data.fd == _server_fdsocket)
            {
                acceptClient();
            }
            else
            {
                handleClientData(events[i].data.fd);
            }
        }
    }
    std::cout << "Server stopped gracefully." << std::endl;
}

void Server::shutdown()
{
    std::cout << "Cleaning up server resources..." << std::endl;
    
    // Close all client connections
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        int client_fd = _clients[i]->get_fd();
        epoll_ctl(epollFd, EPOLL_CTL_DEL, client_fd, NULL);
        close(client_fd);
        std::cout << "Closed client connection: fd " << client_fd << std::endl;
        delete _clients[i];
    }
    _clients.clear();
    
    // Close server socket
    if (_server_fdsocket != -1)
    {
        close(_server_fdsocket);
        _server_fdsocket = -1;
        std::cout << "Server socket closed." << std::endl;
    }
    
    // Close epoll
    if (epollFd != -1)
    {
        close(epollFd);
        epollFd = -1;
        std::cout << "Epoll closed." << std::endl;
    }
}

void Server::setup()
{
    // Register signal handlers
    signal(SIGINT, signalHandler);   // Ctrl+C
    signal(SIGQUIT, signalHandler);  // Ctrl+backslash
    
    std::cout << "Signal handlers registered (SIGINT, SIGQUIT, SIGTERM)" << std::endl;
    
    createSocket();
    setSocketReused();
    setSocketNonBlocking();
    bindSocket();
    listenSocket();
    creatEpoll();
    addSocketToEpoll();
    readSocket();
}






























// Server::Server() : _port(7071), _server_fdsocket(-1){}


// Server::Server(char **av)
// {
//     _fds.resize(MAX_EVENTS);
//     _port = atoi(av[1]);
//     _pass = av[2];
//     _ip = "127.0.0.1";
//     _server_fdsocket = -1;
//     memset(&_server_addr, 0, sizeof(_server_addr));
//     _server_addr.sin_family = AF_INET;
//     _server_addr.sin_addr.s_addr = INADDR_ANY;
//     _server_addr.sin_port = htons(_port);
// }


// void Server::createSocket()
// {
//     _server_fdsocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (_server_fdsocket < 0)
//     {
//         std::cerr << "socket" << std::endl;
//         exit(1);
//     }
    
//     // Enable socket address reuse to avoid "Address already in use" errors
//     int opt = 1;
//     if (setsockopt(_server_fdsocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
//     {
//         std::cerr << "setsockopt SO_REUSEADDR failed" << std::endl;
//         exit(1);
//     }
// }

// void Server::creatEpoll()
// {
//     epollFd = epoll_create1(0);
//     if (epollFd < 0)
//     {
//         std::cerr << "Failed to create epoll instance" << std::endl;
//         exit(1);
//     }
// }


// void Server::bindSocket()
// {
//     int bindRes = bind(_server_fdsocket, (struct sockaddr *)&_server_addr, sizeof(_server_addr));
//     if (bindRes < 0)
//     {
//         std::cerr << "Cant bind IP Address" << std::endl;
//         exit(1);
//     }
// }

// void Server::listenSocket()
// {
//     int lstn = listen(_server_fdsocket, SOMAXCONN);
//     if (lstn < 0)
//     {
//         std::cerr << "Failed to listen on socket" << std::endl;
//         exit(1);
//     }
// }

// void Server::addSocketToEpoll()
// {
//     struct epoll_event ev;
//     ev.events = EPOLLIN;
//     ev.data.fd = _server_fdsocket;
    
//     int res = epoll_ctl(epollFd, EPOLL_CTL_ADD, _server_fdsocket, &ev);
//     if (res < 0)
//     {
//         std::cerr << "Failed to add socket to epoll" << std::endl;
//         exit(1);
//     }
// }

// void Server::closeSocket(int socketFd)
// {
//     close(socketFd);
// }

// void Server::acceptClient()
// {
//     struct sockaddr_in clientAddr;
//     socklen_t addrlen = sizeof(clientAddr);
//     int clientFd = accept(_server_fdsocket, (struct sockaddr *)&clientAddr, &addrlen);
//     if (clientFd < 0)
//     {
//         std::cerr << "Error when accepting client" << std::endl;
//         exit(1);
//     }
//     int res = makeSocketNonBlocking(clientFd);
//     if(res < 0)
//     {
//         std::cerr << "Failed to make client socket non-blocking" << std::endl;
//         closeSocket(clientFd);
//         exit(1);
//     }
    
//     // Create new Client object
//     Client* newClient = new Client();
//     newClient->set_fd(clientFd);
    
//     // Set client IP address
//     char clientIP[INET_ADDRSTRLEN];
//     inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
//     newClient->set_ip_add(std::string(clientIP));
    
//     // Add client to our clients vector
//     _clients.push_back(newClient);
    
//     struct epoll_event event;
//     event.data.fd = clientFd;
//     event.events = EPOLLIN;
    
//     if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &event) < 0)
//     {
//         std::cerr << "Failed to add client socket to epoll" << std::endl;
//         delete newClient;
//         _clients.pop_back();
//         closeSocket(clientFd);
//         exit(1);
//     }
    
//     std::cout << "New client connected: " << clientIP << " (fd: " << clientFd << ")" << std::endl;
// }


// void Server::handleClientData(int clientFd)
// {
//     char buffer[MAX_BUFFER_SIZE];
//     Client* client = _get_client(clientFd);
//     if (!client)
//     {
//         std::cerr << "Client not found for fd: " << clientFd << std::endl;
//         closeSocket(clientFd);
//         epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, NULL);
//         return;
//     }
    
//     int bytesRecv = recv(clientFd, buffer, MAX_BUFFER_SIZE - 1, 0);
//     if (bytesRecv == 0)
//     {
//         std::cout << "Client disconnected (fd: " << clientFd << ")" << std::endl;
//         // Remove client from vector
//         for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
//         {
//             if ((*it)->get_fd() == clientFd)
//             {
//                 delete *it;
//                 _clients.erase(it);
//                 break;
//             }
//         }
//         closeSocket(clientFd);
//         epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, NULL);
//         return;
//     }
//     else if (bytesRecv < 0)
//     {
//         std::cerr << "Error when receiving data from fd: " << clientFd << std::endl;
//         // Remove client from vector
//         for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
//         {
//             if ((*it)->get_fd() == clientFd)
//             {
//                 delete *it;
//                 _clients.erase(it);
//                 break;
//             }
//         }
//         closeSocket(clientFd);
//         epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, NULL);
//         return;
//     }
//     else
//     {
//         // Null-terminate the received data
//         buffer[bytesRecv] = '\0';
//         client->append_to_buffer(std::string(buffer));        
//         std::string clientBuffer = client->get_buffer();
//         std::string::size_type pos = 0;
        
//         while ((pos = clientBuffer.find("\r\n")) != std::string::npos)
//         {
//             std::string completeMessage = clientBuffer.substr(0, pos);
            
//             // Remove the complete message from the buffer            
//             clientBuffer = clientBuffer.substr(pos + 2);
            
//             if (!completeMessage.empty())
//             {
//                 // std::cout << "=== COMPLETE MESSAGE RECEIVED ===" << std::endl;
//                 _exec_cmd(completeMessage.c_str(), clientFd);
//             }
//         }
        
//         // Update client buffer with remaining partial data
//         client->set_buffer(clientBuffer);
        
//         // Show partial buffer information if there's remaining data
//         if (!clientBuffer.empty())
//         {
//             std::cout << "--- PARTIAL BUFFER ---" << std::endl;
//             std::cout << "Client FD: " << clientFd << std::endl;
//             std::cout << "Client IP: " << client->get_ip_address() << std::endl;
//             std::cout << "Partial Buffer: '" << clientBuffer << "'" << std::endl;
//             std::cout << "Buffer Length: " << clientBuffer.length() << " bytes" << std::endl;
//             std::cout << "Waiting for \\r\\n delimiter..." << std::endl;
//             std::cout << "----------------------" << std::endl;
//         }
//     }
// }

// void Server::readSocket()
// {
//     while (1)
//     {
//         // Use the _fds vector instead of local array
//         int nfds = epoll_wait(epollFd, _fds.data(), MAX_EVENTS, -1);
//         if (nfds < 0)
//         {
//             std::cerr << "Error in epoll_wait" << std::endl;
//             exit(1);
//         }
//         for (int i = 0; i < nfds; i++)
//         {
//             if (_fds[i].data.fd == _server_fdsocket)
//             {
//                 // Handle new client connection
//                 acceptClient();
//             }
//             else if (_fds[i].events & EPOLLIN)
//             {
//                 // Handle existing client data
//                 handleClientData(_fds[i].data.fd);
//             }
//         }
//     }
// }

// void Server::setup()
// {
//    this->createSocket();
//    this->creatEpoll();
//    this->bindSocket();
//    this->listenSocket();
//    this->addSocketToEpoll();
//    this->readSocket();
// }