#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

class server{
    private:
        int _server_fd;
        int _client_fd;
        int _port;
        struct sockaddr_in serveraddr;
        struct sockaddr_in clientaddr;
        bool _is_running;
    public:
        server();
        // server(int port);
        // server(const server& other);
        // server& operator=(const server& other);
        void createsocket();
        void bindsocket();
        void listensocket();
        void acceptsocket();
        void runServer();
};