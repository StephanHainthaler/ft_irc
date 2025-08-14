/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 09:22:12 by codespace         #+#    #+#             */
/*   Updated: 2025/08/14 10:52:11 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Client.hpp"
#include "../headers/Server.hpp"


Client::Client() : _socketFD(-1), _port(0), _state(CONNECTING)
{
	_hostname = "localhost";
}

Client::Client(int socketFD, int port)
{
	_socketFD = socketFD; 
	_port = port;
	_state = CONNECTING;
	_nickname = "*";
	_userName = "*";
	_hostname = "localhost";
	_numberOfChannels = 0;
}

Client::~Client() 
{
	// disconnect();
}

void Client::disconnect()
{
	if (_socketFD != -1)
	{
		close(_socketFD);
		_socketFD = -1;
	}
	setState(DISCONNECTED);
}

// NICK/USER/REALNAME CHECKS

bool    Client::isNickValid(const std::string& nickname) const
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


std::string Client::truncName(const std::string& name)
{
	if (name.length() > USERLEN)
		return name.substr(0, USERLEN);
	else
		return (name);
}

void	Client::setNick(const std::string& nickName)
{
	// if (isNickValid(nickName) == 0)
	// 	_nickname = nickName;
	_nickname = nickName;    
}

int		Client::isUserValid(std::string& userName)
{
	if (userName.size() == 0)
		return (/* std::cerr << <client> "<USER> :Not enough parameters" << std::endl, */ 1);
	if (userName.size() > USERLEN)
		userName = truncName(userName);
	return (0);
}

bool	Client::isRealNameValid(const std::string& realName) const
{
	if (realName.length() <= 50)
		return (true);
	return (false);
}

void Client::setUser(std::string& userName, int zero, char asterisk, std::string& realName)
{
	(void) zero;
	(void) asterisk;
	if (isUserValid(userName) == 0)
		_userName = userName;
	if (isRealNameValid(realName) == 0)
		_realName = realName;
	else
		_realName = realName.substr(0, 50);
}

void	Client::isFullyRegistered()
{
	 if (!_userName.empty() && !_nickname.empty() && !_realName.empty())
		setState(REGISTERED);
}

std::string Client::getFullIdentifier() const
{
	return (_nickname + "!" + _userName + "@" + _hostname);
}

// CHANNELS


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
			_numberOfChannels -= number;
	}
}





// MODES

bool Client::isValidUserMode(char mode) const
{
	// Standard IRC user modes
	const std::string validModes = "ioOwr";  // Add more as needed
	if (validModes.find(mode) != std::string::npos)
   		return (true);
	return (false);
}

int Client::setMode(char mode, bool enable)
{
	if (!isValidUserMode(mode))
	{
		
		std::cerr << "Error: Invalid user mode '" << mode << "'" << std::endl;
		return (1) ;
	}
	if (enable)
	{
		if (_modes.find(mode) == std::string::npos)
			_modes += mode;
	}
	else
	{
		// Disable/remove mode if "false" passed as boolean
		std::string::size_type pos = _modes.find(mode);
		if (pos != std::string::npos)
			_modes.erase(pos, 1);
	}
	return (0);
}

bool Client::hasMode(char mode) const
{
	if (_modes.find(mode) != std::string::npos)
   		return (true);
	return (false);
}

std::string Client::getModes() const
{
	if (_modes.empty())
		return ("");
	return ("+" + _modes);
}
/* Client::Client(int socketFD, const sockaddr_in& clientAddr) : 
	_socketFD(socketFD),
	_state(CONNECTING),
	_channels(CHANLIMIT)
{
} */
void Client::setState(ClientState newState)
{ 
	_state = newState; 
}
ClientState Client::getState() const
{ 
	return (_state);
}

std::string Client::getNickname() const
{
	return (_nickname);
}

std::string Client::getUsername() const
{
	return (_userName);
}

std::string Client::getRealname() const
{
	return (_realName);
}

void Client::setSocketFD(int socketFD)
{
	_socketFD = socketFD;
}

int Client::getSocketFD() const
{
	return (_socketFD);
}

void Client::setIP(const std::string& ip)
{
	_IP = ip;
}

std::string Client::getIP() const
{
	return (_IP);
}

void Client::setHostname(const std::string& hostname)
{
	_hostname = hostname;
}

std::string Client::getHostname() const
{
	return (_hostname);
}

size_t	Client::getChannelNumber() const
{
	return (_numberOfChannels);
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
