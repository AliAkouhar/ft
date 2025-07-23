#include "../inc/Server.hpp"

#define MAX_EVENTS 20
#define BUFFER_SIZE 4096

Server::Server() : _serverFd(-1), _port(8080) {}

Server::Server(char **av)
{
    _port = atoi(av[1]);
    _password = av[2];
    _ip = "127.0.0.1";
    _serverFd = -1;
    memset(&_serverAddr, 0, sizeof(_serverAddr));
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_addr.s_addr = inet_addr(_ip.c_str());
    _serverAddr.sin_port = htons(_port);
}

void Server::createSocket()
{
    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd < 0)
    {
        std::cerr << "socket" << std::endl;
        exit(1);
    }
}

void Server::setSocketReused()
{
    int opt = 1;
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(1);
    }
}

void Server::bindSocket()
{
    if (bind(_serverFd, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) < 0)
    {
        perror("bind");
        exit(1);
    }
}

void Server::listenSocket()
{
    if (listen(_serverFd, SOMAXCONN) < 0)
    {
        perror("listen");
        exit(1);
    }
}

int Server::makeSocketNonBlocking(int sFd)
{
    int flag, s;

    if (flag = fcntl(sFd, F_GETFL, 0) < 0)
    {
        perror("fcntl");
        return -1;
    }
    flag |= O_NONBLOCK;
    if (s = fcntl(sFd, F_SETFL, flag) < 0)
    {
        perror("fcntl");
        return -1;
    }
    return 0;
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


void Server::addEpoll(int fd)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        std::cerr << "Failed to add file descriptor to epoll" << std::endl;
        exit(1);
    }
}

void Server::acceptClient()
{
    struct sockaddr_in clientAddr;
    socklen_t addrlen = sizeof(clientAddr);
    int clientFd = accept(_serverFd, (struct sockaddr *)&clientAddr, &addrlen);
    if (clientFd < 0)
    {
        std::cerr << "Errir when accepting client" << std::endl;
        exit(1);
    }
    makeSocketNonBlocking(clientFd);
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
            if (events[i].data.fd == _serverFd)
                acceptClient();
            else
            {
                char buffer[BUFFER_SIZE];
                if (events[i].events & EPOLLIN)
                {
                    int totalBytesRecieved = 0;
                    int bytesRecv = recv(events[i].data.fd, buffer + totalBytesRecieved, BUFFER_SIZE - totalBytesRecieved, 0);
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
                        // if ()
                        // buffer + '\0'; + fd of the client that triger the event
                        std::cout << "buffer : " << buffer << std::endl;
                    }
                }
            }
        }
    }

}

void Server::closeSocket(int socketFd)
{
    if (socketFd >= 0)
        close(socketFd);
}
{
    close(_serverFd);
}