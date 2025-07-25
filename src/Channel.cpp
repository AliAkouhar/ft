#include "../inc/Channel.hpp"

Channel::Channel()
{
	_limit = -1;
	_has_key = false;
	_invite_only = false;
	_topic_restriction = false;
	_key = "";
	_name = "";
	_topic = "";
	_created_at = "";
	_clients = std::vector<Client*>();
	_operator_clients = std::vector<Client*>();
}

Channel::Channel(std::string name)
{
	_limit = -1;
	_has_key = false;
	_invite_only = false;
	_topic_restriction = false;
	_key = "";
	_name = name;
	_topic = "";
	_created_at = "";
	_clients = std::vector<Client*>();
	_operator_clients = std::vector<Client*>();
}

Channel::~Channel()
{
	_clients.clear();
	_operator_clients.clear();
}

std::string Channel::get_name(void) const { return _name; }

bool Channel::get_topic_restriction(void) const { return _topic_restriction; }

std::vector<Client*> Channel::get_operator_clients(void)
{
	return this->_operator_clients;
}

std::vector<Client*> Channel::get_channel_clients(void)
{
	return this->_clients;
}

std::string Channel::get_client_names(void) const
{
	std::string names;
	for (std::vector<Client*>::const_iterator it = this->_clients.begin();
		 it != this->_clients.end();
		 ++it)
	{
		names += (*it)->get_nickname();
		names += " ";
	}
	return names;
}

int Channel::get_clients_size(void) const { return this->_clients.size(); }

std::string Channel::get_topic(void) const { return _topic; }

std::string Channel::get_channel_key(void) const { return _key; }

void Channel::set_channel_operator(Client* client)
{
	client->set_is_operator(true);
	this->_operator_clients.push_back(client);
	return;
}

void Channel::set_topic(std::string topic) { _topic = topic; }

void Channel::set_limit(int limit) { _limit = limit; }

void Channel::set_invite_only() { _invite_only = true; }

void Channel::set_topic_restriction() { _topic_restriction = true; }

void Channel::set_key(std::string key) { _key = key; }

void Channel::remove_limit(void) { _limit = -1; }

void Channel::remove_key(void) { _key = ""; }

void Channel::remove_topic_restriction(void) { _topic_restriction = false; }

void Channel::remove_invite_only(void) { _invite_only = false; }

void Channel::remove_channel_operator(Client* client)
{
	client->set_is_operator(false);
	for (std::vector<Client*>::iterator it = this->_operator_clients.begin();
		 it != this->_operator_clients.end();
		 ++it)
	{
		if ((*it)->get_nickname() == client->get_nickname())
		{
			this->_operator_clients.erase(it);
			return;
		}
	}
}

void Channel::remove_channel_client(Client* client)
{
	std::vector<Client*>::iterator it =
		std::remove(_clients.begin(), _clients.end(), client);
	_clients.erase(it, _clients.end());
}

bool Channel::has_client(Client* client)
{
	for (std::vector<Client*>::const_iterator it = this->_clients.begin();
		 it != this->_clients.end();
		 ++it)
	{
		if ((*it)->get_nickname() == client->get_nickname())
			return true;
	}
	return false;
}

bool Channel::has_key(void) const { return _key != ""; }

bool Channel::is_channel_operator(std::string nickname)
{
	for (std::vector<Client*>::iterator it = _operator_clients.begin();
		 it != this->_operator_clients.end();
		 ++it)
	{
		if ((*it)->get_nickname() == nickname)
			return true;
	}
	return false;
}

bool Channel::is_channel_operator(const int fd)
{
	for (std::vector<Client*>::iterator it = _operator_clients.begin();
		 it != this->_operator_clients.end();
		 ++it)
	{
		if ((*it)->get_fd() == fd)
			return true;
	}
	return false;
}

void Channel::invite(Client* client)
{
	_clients.push_back(client);
	return;
}

void Channel::join(Client* client)
{
	_clients.push_back(client);
	return;
}

void Channel::kick(Client* client)
{
	client->remove_channel_invited(this->get_name());
	remove_channel_operator(client);
	remove_channel_client(client);
}

void Channel::part(Client* client)
{
	remove_channel_operator(client);
	remove_channel_client(client);
}

void Channel::quit(Client* client) { remove_channel_operator(client); }

void Channel::broadcast(Client* sender, std::string target, std::string message)
{
	for (std::vector<Client*>::iterator it = this->_clients.begin();
		 it != this->_clients.end();
		 it++)
	{
		if (*it == sender)
			continue;
		(*it)->broadcast(sender, target, message);
	}
}

bool Channel::is_client_in_channel(std::string nickname)
{
	for (std::vector<Client*>::iterator it = this->_clients.begin();
		 it != this->_clients.end();
		 ++it)
	{
		if ((*it)->get_nickname() == nickname)
			return true;
	}
	return false;
}

bool Channel::is_channel_full(void) const
{
	if (_limit == -1)
		return false;
	if (_clients.size() >= static_cast<size_t>(_limit))
		return true;
	return false;
}

bool Channel::is_channel_invite_only(void) const { return _invite_only; }
