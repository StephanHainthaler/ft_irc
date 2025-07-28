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