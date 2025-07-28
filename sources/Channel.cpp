/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shaintha <shaintha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 17:40:33 by shaintha          #+#    #+#             */
/*   Updated: 2025/07/09 17:40:33 by shaintha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Channel.hpp"

Channel::Channel(const std::string &name, const std::string &topic, const std::string &modes)
	: _name(name), _topic(topic), _modes(modes)
{
}

Channel::~Channel(void)
{
	_channelUsers.clear();
	_operators.clear();
}

std::string Channel::getName(void) const
{
	return _name;
}

std::string	Channel::getTopic(void) const
{
	return _topic;
}

void	Channel::setTopic(std::string topic)
{
	_topic = topic;
}

std::string Channel::getModes(void) const
{
	return _modes;
}

bool Channel::isValidChannelMode(char mode) const
{
	return (mode == 'i' || mode == 't' || mode == 'k' || mode == 'o' || mode == 'l');
}

int Channel::setMode(char mode, bool enable)
{
	if (isValidChannelMode(mode))
	{
		if (enable && _modes.find(mode) == std::string::npos)
			_modes += mode;
		else
		{ // Disable/remove mode if "false" passed as boolean
			std::string::size_type pos = _modes.find(mode);
			if (pos != std::string::npos)
				_modes.erase(pos, 1);
		}
		return (0);
	}
	else
		return (ERR_UNKNOWNMODE) ;
}

bool Channel::hasMode(char mode) const
{
	return (_modes.find(mode) != std::string::npos);
}

std::vector<Client *> Channel::getChannelUsers(void) const
{
	return _channelUsers;
}

std::vector<Client *> Channel::getOperators(void) const
{
	return _operators;
}

Client *Channel::getUser(const std::string &nickname) const
{
	for (std::vector<Client *>::const_iterator it = _channelUsers.begin(); it != _channelUsers.end(); ++it)
	{
		if ((*it)->getNickname() == nickname)
			return *it;
	}
	return NULL;
}

void Channel::addUser(Client *client)
{
	if (client && std::find(_channelUsers.begin(), _channelUsers.end(), client) == _channelUsers.end())
		_channelUsers.push_back(client);
}

void Channel::removeUser(Client *client)
{
	if (client)
	{
		std::vector<Client *>::iterator it = std::find(_channelUsers.begin(), _channelUsers.end(), client);
		if (it != _channelUsers.end())
			_channelUsers.erase(it);
	}
}

void Channel::addOperator(Client *client)
{
	if (client && std::find(_operators.begin(), _operators.end(), client) == _operators.end())
		_operators.push_back(client);
}

void Channel::removeOperator(Client *client)
{
	if (client)
	{
		std::vector<Client *>::iterator it = std::find(_operators.begin(), _operators.end(), client);
		if (it != _operators.end())
			_operators.erase(it);
	}
}