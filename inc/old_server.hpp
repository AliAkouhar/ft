#ifndef OLD_SERVER_HPP
#define OLD_SERVER_HPP

# include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <fcntl.h>
#include <sys/epoll.h>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <algorithm>
#include <fstream>
#include "client.hpp"
class client;
#include "client.hpp" // Ensure the full definition of client is included

class Server {
public:
    Server(int port);
    ~Server();
    void run();

private:
    int _serverFd;
    int _port;
    int epollFd;
    std::string _password;
    std::string _ip;
    struct sockaddr_in _serverAddr;
    std::map<int, client> _clients; // Map of client file descriptors to client objects
    Server();
    Server(char **av);
    Server(const Server &other);
    Server &operator=(const Server &other);

    void createSocket();
    void setSocketReused();
    void bindSocket();
    void listenSocket();
    static int makeSocketNonBlocking(int sFd);
    void closeSocket(int socketFd);
    void creatEpoll();
    void addEpoll(int fd);
    void acceptClient();
    void readSocket();
    void setup();
};;

#endif
