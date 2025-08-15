/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shaintha <shaintha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 09:22:12 by juitz             #+#    #+#             */
/*   Updated: 2025/08/15 09:44:43 by shaintha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Client.hpp"

Client::Client(int socketFD) : _socketFD(socketFD)
{
	_state = CONNECTING;
	_nickname = "*";
	_userName = "*";
	_hostname = "localhost";
	_numberOfChannels = 0;
}

Client::~Client(void) 
{
}

std::string	Client::getClientName(void) const
{
	std::string clientName = "*";
	
	if (getNickname() != "*")
	{
		clientName = getNickname();
		if (getUsername() != "*")
		{
			clientName += "!" + getUsername();
			if (getHostname().empty() == false)
				 clientName += "@" + getHostname();
		}
	}
	return (clientName);
}

void	Client::setNickname(const std::string& nickName)
{
	_nickname = nickName;    
}

std::string	Client::getNickname(void) const
{
	return (_nickname);
}

void	Client::setUsername(const std::string &userName)
{
	if (isUsernameValid(userName) == false)
		_userName = userName.substr(0, USERLEN);
	else
		_userName = userName;
}

std::string Client::getUsername(void) const
{
	return (_userName);
}

void Client::setHostname(const std::string& hostname)
{
	_hostname = hostname;
}

std::string Client::getHostname() const
{
	return (_hostname);
}

void	Client::setRealname(const std::string &realName)
{
	if (isRealnameValid(realName) == false)
		_realName = realName.substr(0, 50);
	else
		_realName = realName;
}

std::string	Client::getRealname(void) const
{
	return (_realName);
}

void	Client::setSocketFD(int socketFD)
{
	_socketFD = socketFD;
}

int	Client::getSocketFD(void) const
{
	return (_socketFD);
}

void	Client::setIP(const std::string& ip)
{
	_IP = ip;
}

std::string	Client::getIP() const
{
	return (_IP);
}

void	Client::setState(ClientState newState)
{ 
	_state = newState; 
}
ClientState Client::getState() const
{ 
	return (_state);
}

void	Client::setChannelNumber(int number)
{
	if (number >= 0)
	{
		if ( _numberOfChannels != CHANLIMIT)
			_numberOfChannels += number;
	}
	else
	{
		if ( _numberOfChannels != 0)
			_numberOfChannels += number;
	}
}

size_t	Client::getChannelNumber() const
{
	return (_numberOfChannels);
}

bool	Client::isNicknameValid(const std::string& nickname) const
{
	if (nickname.size() > 9 || nickname.size() == 0)
		return (false);
	if (nickname[0] == '$' || nickname[0] == ':' || nickname[0] == '#' || nickname[0] == '~' || nickname[0] == '&' || nickname[0] == '+')
		return (false);
	for (size_t i = 0; i < nickname.size(); i++)
		if (nickname[i] == ' ' || nickname[i] == ',' || nickname[i] == '*' || nickname[i] == '?' || nickname[i] == '!' || nickname[i] == '@')
			return (false);
	return (true);
}

bool	Client::isUsernameValid(const std::string &userName)
{
	if (userName.length() > USERLEN)
		return (false);
	return (true);
}

bool	Client::isRealnameValid(const std::string& realName) const
{
	if (realName.length() <= 50)
		return (true);
	return (false);
}

void	Client::disconnect(void)
{
	if (_socketFD != -1)
	{
		close(_socketFD);
		_socketFD = -1;
	}
	setState(DISCONNECTED);
}
