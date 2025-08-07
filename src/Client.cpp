#include "../inc/Client.hpp"

Client::Client()
{
	_fd = -1;
	_is_logged = false;
	_is_authenticated = false;
	_is_operator = false;
	_buffer = "";
	_ip_addr = "";
	_nickname = "";
	_username = "";
	_password = "";
	_channels_invited = std::vector<std::string>();
}

Client::Client(const Client& other)
{
	_fd = other._fd;
	_is_logged = other._is_logged;
	_is_authenticated = other._is_authenticated;
	_is_operator = other._is_operator;
	_buffer = other._buffer;
	_ip_addr = other._ip_addr;
	_nickname = other._nickname;
	_username = other._username;
	_password = other._password;
	_channels_invited = other._channels_invited;
}

Client::~Client() {
	if (_fd != -1) {
		close(_fd);
		_fd = -1;
	}
}

void Client::set_fd(const int fd) { _fd = fd; }

void Client::set_ip_add(const std::string& ipadd) { _ip_addr = ipadd; }

std::string Client::get_ip_address() const { return _ip_addr; }

void Client::set_buffer(const std::string& buffer) { _buffer = buffer; }

void Client::set_nickname(const std::string& nickname) { _nickname = nickname; }

void Client::set_username(const std::string& username) { _username = username; }

void Client::set_password(const std::string& password) { _password = password; }

void Client::set_is_authenticated(bool is_authenticated) {	_is_authenticated = is_authenticated;}

void Client::set_is_operator(bool is_operator) { _is_operator = is_operator; }

void Client::set_is_logged(bool is_logged) { _is_logged = is_logged; }

bool Client::get_is_authenticated() const { return _is_authenticated; }

bool Client::get_is_operator() const { return _is_operator; }

bool Client::get_is_logged() const { return _is_logged; }

int Client::get_fd() const { return _fd; }

std::string Client::get_buffer() const { return _buffer; }

std::string Client::get_nickname() const { return _nickname; }

std::string Client::get_username() const { return _username; }

std::string Client::get_password() const { return _password; }

std::string Client::get_hostname() const { 
	return _username + "@" + _ip_addr;
}

std::vector<std::string> Client::get_channels_invited() const {	return _channels_invited; }

bool Client::get_already_registered() const { return _username.empty(); }

void Client::add_channel_invited(const std::string& channel) {	_channels_invited.push_back(channel); }

void Client::append_to_buffer(const std::string& data) { _buffer += data; }

void Client::clear_buffer(void) { _buffer.clear(); }

bool Client::is_channel_invited(const std::string& channel)
{
	return std::find(_channels_invited.begin(),
					 _channels_invited.end(),
					 channel) != _channels_invited.end();
}

void Client::remove_channel_invited(const std::string& channel)
{
	std::vector<std::string>::iterator it;
	for (it = _channels_invited.begin(); it != _channels_invited.end(); ++it)
	{
		if (*it == channel)
		{
			_channels_invited.erase(it);
			break;
		}
	}
}

void Client::broadcast(Client* sender, std::string target, std::string message)
{
	std::string response = RPL_PRIVMSG(
		sender->get_nickname(), sender->get_hostname(), target, message);

	if (send(this->get_fd(), response.c_str(), response.size(), 0) == -1)
		std::cerr << "Response sending failed" << std::endl;
}
