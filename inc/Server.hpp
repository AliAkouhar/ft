#ifndef SERVER_HPP
#define SERVER_HPP

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <sys/epoll.h>

//-------------------------------------------------------//

#define RED "\e[1;31m"
#define WHI "\e[0;37m"
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"

//-------------------------------------------------------//

#include "../inc/Channel.hpp"
#include "../inc/Client.hpp"
#include "../inc/Replies.hpp"

#define SPACE " "
#define CRLF "\r\n"
#define LINE_FEED "\n"
#define DELIMITER " \t"

class Server
{
  public:
	Server();
	~Server();
	Server(char **ac);

	void createSocket();
    void setSocketReused();
    void bindSocket();
    void listenSocket();
    void closeSocket(int socketFd);
    void creatEpoll();
    void addSocketToEpoll();
    void acceptClient();
    void readSocket();
    void handleClientData(int clientFd);
    void setup();
  private:
	int _port;
	int _server_fdsocket;
	int epollFd;
	std::string _hostname;
	std::string _pass;
	std::string _ip;
	std::vector<struct epoll_event> _fds;
	struct sockaddr_in _server_addr;
	std::vector<Client*> _clients;
	std::vector<Channel*> _channels;

	int _reply_code;

	std::string _get_hostname();

	// void _is_valid_port(const std::string& port);
	// bool _is_client_in_any_channel(const int fd);
	bool _client_is_ready_to_login(const int fd);
	bool _is_valid_nickname(const std::string& nickname);
	bool _is_nickname_in_use(const int fd, const std::string& nickname);
	// COMMAND HANDLING COMMENTED OUT - TO BE HANDLED BY PARTNER
	// void _handle_command(const std::string& command, const std::string& parameters, const int fd);
	void _handler_client_join(const std::string& buffer, const int fd);
	void _handler_client_quit(const std::string& buffer, const int fd);
	void _handler_client_part(const std::string& buffer, const int fd);
	void _handler_client_mode(const std::string& buffer, const int fd);
	void _handler_client_kick(const std::string& buffer, const int fd);
	void _handler_client_topic(const std::string& buffer, const int fd);
	void _handler_client_invite(const std::string& buffer, const int fd);
	void _handler_client_privmsg(const std::string& buffer, const int fd);
	void _handler_client_nickname(const std::string& nickname, const int fd);
	void _handler_client_username(const std::string& username, const int fd);
	void _handler_client_password(const std::string& password, const int fd);
	void _send_response(const int fd, const std::string& response);

	// static bool _signal;
	// void _clear_client(const int fd);

	// COMMAND EXECUTION COMMENTED OUT - TO BE HANDLED BY PARTNER
	// void _execute_command(const std::string buffer, const int fd);

	std::string _cleanse_buffer(const std::string& buffer,
								const std::string& chars_to_remove);
	std::vector<std::string> _split_buffer(const std::string& buffer,
										   const std::string& delimiter);

	Client* _get_client(const int fd);
	Client* _get_client(const std::string nickname);

	void _add_channel(Channel* channel);
	Channel* _get_channel(const std::string& channel_name);

	void _remove_client_fd(const int fd);
	void _remove_client_from_channels(const int fd);
	void _remove_client_from_server(const int fd);
};

std::string toupper(const std::string& str);

#endif