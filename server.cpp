#include "server.hpp"

server::server() : _server_fd(-1), _client_fd(-1), _port(6667), _is_running(false)
{
    this->serveraddr.sin_addr.s_addr = INADDR_ANY;
    this->serveraddr.sin_family = AF_INET;
    this->serveraddr.sin_port = htons(6667);
    memset(&this->serveraddr.sin_zero, 0, sizeof(this->serveraddr.sin_zero)); 
}

// server::server(int port) : _server_fd(-1), _client_fd(-1), _port(port), _is_running(false)
// {
//     memset(&serveraddr, 0, sizeof(serveraddr));
//     memset(&clientaddr, 0, sizeof(clientaddr));
// }

// server::server(const server& other) : _server_fd(-1), _client_fd(-1),_port(other._port) , _is_running(false)
// {
//     memset(&serveraddr, 0, sizeof(serveraddr));
//     memset(&clientaddr, 0, sizeof(clientaddr));
// }

// server& server::operator=(const server& other)
// {
//     _server_fd = -1;
//     _client_fd = -1;
//     _port = other._port;
//     _is_running = false;
//    memset(&serveraddr, 0, sizeof(serveraddr));
//    memset(&clientaddr, 0, sizeof(clientaddr)); 
// }

void server::createsocket()
{
    if (this->_server_fd = socket(AF_INET, SOCK_STREAM, 0) < 0)
    {
        std::cerr << "Error setting socket\n";
        return ;
    }
}

void server::bindsocket()
{
    if (bind(_server_fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
    {
       std::cerr << "Error while binding\n";
       return;
    }
}

void server::listensocket()
{
    if (listen(_server_fd, 1) < 0)
    {
        std::cerr << "Error while listening\n";
        return ;
    }
}

