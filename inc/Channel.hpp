#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"
#include "Replies.hpp"

class Channel
{
  public:
	Channel();
	~Channel();
	Channel(std::string name);

	int get_clients_size(void) const;
	std::string get_name(void) const;
	std::string get_topic(void) const;
	std::string get_channel_key(void) const;
	std::string get_client_names(void) const;
	std::vector<Client*> get_channel_clients(void);
	std::vector<Client*> get_operator_clients(void);
	bool get_topic_restriction(void) const;

	void set_limit(int limit);
	void set_invite_only(void);
	void set_topic_restriction(void);
	void set_topic(std::string topic);
	void set_key(std::string password);
	void set_channel_operator(Client* client);

	void remove_key(void);
	void remove_limit(void);
	void remove_invite_only(void);
	void remove_topic_restriction(void);
	void remove_channel_operator(Client* client);
	void remove_channel_client(Client* client);

	void join(Client* client);
	void kick(Client* client);
	void part(Client* client);
	void quit(Client* client);
	void invite(Client* client);
	void broadcast(Client* sender, std::string target, std::string message);

	bool has_key(void) const;
	bool has_client(Client* client);

	bool is_channel_full(void) const;
	bool is_channel_operator(const int fd);
	bool is_channel_invite_only(void) const;
	bool is_channel_operator(std::string nickname);
	bool is_client_in_channel(std::string nickname);

  private:
	int _limit;
	bool _has_key;
	bool _invite_only;
	bool _topic_restriction;
	std::string _key;
	std::string _name;
	std::string _topic;
	std::string _created_at;
	std::vector<Client*> _clients;
	std::vector<Client*> _operator_clients;
};

#endif