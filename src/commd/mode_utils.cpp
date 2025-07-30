#include "../../inc/Server.hpp"

bool _process_mode_flags(const std::string& modeFlags, Channel* channel,
						 Client* targetClient, const std::string& argument)
{
	bool addMode = false;
	char mode = 0;

	for (size_t i = 0; i < modeFlags.size(); ++i)
	{
		char flag = modeFlags[i];
		if (flag == '+')
			addMode = true;
		else if (flag == '-')
			addMode = false;
		else
		{
			mode = flag;
			if (!_apply_mode_flag(
					channel, targetClient, mode, addMode, argument))
				return false;
		}
	}
	return true;
}

bool _apply_mode_flag(Channel* channel, Client* targetClient, char mode,
					  bool addMode, const std::string& argument)
{
	switch (mode)
	{
	case 'i':
		_set_invite_only_mode(channel, addMode);
		break;
	case 't':
		_set_topic_restriction_mode(channel, addMode);
		break;
	case 'k':
		_set_channel_key_mode(channel, argument, addMode);
		break;
	case 'o':
		_set_channel_operator_mode(channel, targetClient, addMode);
		break;
	case 'l':
		_set_channel_limit_mode(channel, argument, addMode);
		break;
	default:
		return false;
	}
	return true;
}

void _set_invite_only_mode(Channel* channel, bool addMode)
{
	if (addMode)
		channel->set_invite_only();
	else
		channel->remove_invite_only();
}

void _set_topic_restriction_mode(Channel* channel, bool addMode)
{
	if (addMode)
		channel->set_topic_restriction();
	else
		channel->remove_topic_restriction();
}

void _set_channel_key_mode(Channel* channel, const std::string& key,
						   bool addMode)
{
	if (addMode)
		channel->set_key(key);
	else
		channel->remove_key();
}

void _set_channel_operator_mode(Channel* channel, Client* client, bool addMode)
{
	if (addMode && client)
		channel->set_channel_operator(client);
	else if (client)
		channel->remove_channel_operator(client);
}

void _set_channel_limit_mode(Channel* channel, const std::string& limitStr,
							 bool addMode)
{
	if (addMode)
	{
		if (!limitStr.empty())
			channel->set_limit(std::atoi(limitStr.c_str()));
	}
	else
		channel->remove_limit();
}
