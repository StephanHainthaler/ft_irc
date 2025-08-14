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


Channel::Channel(const std::string &name, Client &creator): _name(name)
{
	_userLimit = 1;
	_channelKey = "";
	_topic = "";
	_modes = "";
	_operators.push_back(&creator);
	creator.setChannelNumber(1);
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
	return (mode == 'i' || mode == 't' || mode == 'k' || mode == 'l');
}

int Channel::setMode(char mode, std::string modearg, bool doEnable, std::string &comment)
{
	if (doEnable == true)
	{
		if ((mode == 'o' || mode == 'k' || mode == 'l') && modearg.empty())
			return (comment = "Empty Argument", 1);
		else if (mode == 'o')
		{
			if (setOperator(modearg, doEnable) == 1)
				return (comment = "User does NOT exist on channel", 1);
		}	
		else if (mode == 'k')
			_channelKey = modearg;
		else if (mode == 'l')
		{
			if (isPositiveNumber(modearg) == false)
				return (comment = "Invalid number", 1);
			_userLimit = std::atoi(modearg.c_str());
		}
		if (hasMode(mode) == false)
			_modes += mode;
	}
	else
		if (hasMode(mode) == true)
			_modes.erase(_modes.find(mode), 1);
	return (0);
}

bool Channel::hasMode(char mode) const
{
	return (_modes.find(mode) != std::string::npos);
}

std::string Channel::getModeArguments(void) const
{
	std::string args = "";
	for (size_t i = 0; i < _modes.size(); ++i)
	{
		if (_modes[i] == 'k')
			args += _channelKey + " ";
		else if (_modes[i] == 'l')
		{
			std::ostringstream oss;
			oss << _userLimit;
			args += oss.str() + " ";
		}
		else if (_modes[i] == 'o')
		{
			for (size_t j = 0; j < _operators.size(); ++j)
			{
				args += _operators[j]->getNickname();
				args += " ";
			}
		}
	}
	return args;
}

std::vector<Client *> Channel::getChannelUsers(void) const
{
	return _channelUsers;
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
		if (isOperator(client) == true)
		{
			std::vector<Client *>::iterator it = std::find(_operators.begin(), _operators.end(), client);
			if (it != _operators.end())
				_operators.erase(it);
		}
		else
		{
			std::vector<Client *>::iterator it = std::find(_channelUsers.begin(), _channelUsers.end(), client);
			if (it != _channelUsers.end())
				_channelUsers.erase(it);
		}
	}
}

Client *Channel::getUser(const std::string &nickname) const
{
	for (std::vector<Client *>::const_iterator it =_operators.begin(); it !=_operators.end(); ++it)
		if ((*it)->getNickname() == nickname)
			return *it;
	for (std::vector<Client *>::const_iterator it = _channelUsers.begin(); it != _channelUsers.end(); ++it)
		if ((*it)->getNickname() == nickname)
			return *it;
	return NULL;
}

std::vector<Client *> Channel::getOperators(void) const
{
	return _operators;
}

bool Channel::isOperator(Client *client) const
{
	if (!client)
		return false;
	return (std::find(_operators.begin(), _operators.end(), client) != _operators.end());
}

int	Channel::setOperator(std::string &nickname, bool enable)
{
	
	Client *client = getUser(nickname);
	if (!client)
		return (1);

	if (enable && std::find(_operators.begin(), _operators.end(), client) == _operators.end())
	{
		_operators.push_back(client);
		std::vector<Client *>::iterator it = std::find(_channelUsers.begin(), _channelUsers.end(), client);
		if (it != _channelUsers.end())
			_channelUsers.erase(it);
	}
	else if (!enable)
	{
		_channelUsers.push_back(client);
		std::vector<Client *>::iterator it = std::find(_operators.begin(), _operators.end(), client);
		if (it != _operators.end())
			_operators.erase(it);
	}
	return (0);
}

std::string Channel::getChannelKey(void)
{
	return _channelKey;
}

void Channel::setChannelKey(const std::string &key)
{
	_channelKey = key;
}

unsigned int Channel::getUserLimit(void) const
{
	return _userLimit;
}

void Channel::setUserLimit(unsigned int limit)
{
	_userLimit = limit;
}

//ADDED BY STEPHAN
std::string	Channel::getNamesOfChannelMembers(void) const
{
	std::string namesOfUsers = "";

	for (size_t i = 0; i < _operators.size(); i++)
	{
		namesOfUsers += "@" + _operators[i]->getNickname();
		if ((i + 1) != _operators.size())
			namesOfUsers += " ";
	}
	if (_channelUsers.size() > 0)
		namesOfUsers += " ";
	for (size_t i = 0; i < _channelUsers.size(); i++)
	{
		namesOfUsers += _channelUsers[i]->getNickname();
		if ((i + 1) != _channelUsers.size())
			namesOfUsers += " ";
	}
	return (namesOfUsers);
}
