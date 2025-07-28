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

Channel::Channel(const std::string &name, const std::string &topic, const std::string &state, const std::string &mode)
	: _name(name), _topic(topic), _state(state), _mode(mode)
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

std::string Channel::getState(void) const
{
	return _state;
}

void Channel::setState(std::string state)
{
	_state = state;
}

std::string Channel::getMode(void) const
{
	return _mode;
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