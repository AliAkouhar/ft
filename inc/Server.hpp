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
	void setSocketNonBlocking();
    void shutdown();
	int _get_port(std::string port);
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

	void _handle_commands(const std::string& command, const std::string& parameters, const int fd);
	bool _is_nickname_in_use(const int fd, const std::string& nickname);
	bool _is_valid_nickname(const std::string& nickname);
	bool _client_is_ready_to_login(const int fd);
	void _clear_client(const int fd);

	void _ft_join(const std::string& buffer, const int fd);
	void _ft_quit(const std::string& buffer, const int fd);
	void _ft_part(const std::string& buffer, const int fd);
	void _ft_mode(const std::string& buffer, const int fd);
	void _ft_kick(const std::string& buffer, const int fd);
	void _ft_topic(const std::string& buffer, const int fd);
	void _ft_invite(const std::string& buffer, const int fd);
	void _ft_privmsg(const std::string& buffer, const int fd);
	void _ft_nickname(const std::string& nickname, const int fd);
	void _ft_username(const std::string& username, const int fd);
	void _ft_password(const std::string& password, const int fd);
	void _send_response(const int fd, const std::string& response);
	void _exec_cmd(const std::string buffer, const int fd);

	int _user_checks(Client *client, const int fd);
	void _part_cont(std::string& channel_name, Client* client, const int fd);
	int _nickname_checks(const std::string& nickname, Client* client, const int fd);
	int _join_checks(Client *client, const int fd, const std::vector<std::string>& params);
	int _kick_checks(Client* kicker, Channel* channel, const int fd, const std::string& channel_name, std::vector<std::string> params);
	int _topic_checks(Client* client, const int fd, std::string channel_name, std::vector<std::string>& params);
	int _mode_checks(Client* client, const int fd, const std::string& channel_name,
						const std::string& modeFlags);
	int _privmsg_checks(Client* client, const int fd, std::vector<std::string> receivers);					
	int _invite_checks(Client* client, Client* invitee ,const int fd, std::vector<std::string>& params);

				
	std::string _remove_rn(const std::string& buffer,
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

bool _process_mode_flags(const std::string& modeFlags, Channel* channel,
						 Client* targetClient, const std::string& argument);
bool _apply_mode_flag(Channel* channel, Client* targetClient, char mode,
					  bool addMode, const std::string& argument);
void _set_invite_only_mode(Channel* channel, bool addMode);
void _set_topic_restriction_mode(Channel* channel, bool addMode);
void _set_channel_key_mode(Channel* channel, const std::string& key,
						   bool addMode);
void _set_channel_operator_mode(Channel* channel, Client* client, bool addMode);
void _set_channel_limit_mode(Channel* channel, const std::string& limitStr,
							 bool addMode);
							
std::string toupper(const std::string& str);

#endif