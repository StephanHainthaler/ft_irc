/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 09:22:12 by codespace         #+#    #+#             */
/*   Updated: 2025/07/17 10:45:49 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Client.hpp"
#include "../headers/main.hpp"

// Will have to send the error messages from server, but I will keep it for now for reference

int	Client::isNickValid(const std::string& nickname) const
{
	if (nickname.size() > 9 || nickname.size() == 0)
			return (std::cout << "Error: Nickname must be at least 1 character and can only be max 9 characters long." << std::endl, ERR_ERRONEUSNICKNAME);
	if (nickname[0] == '$' || nickname[0] == ':' || nickname[0] == '#' || nickname[0] == '~' || nickname[0] == '&' || nickname[0] == '+')
			return (std::cout << "Error: Nickname invalid." << std::endl, ERR_ERRONEUSNICKNAME);

	for (size_t i = 0; i < nickname.size(); i++)
	{
		if (nickname[i] == ' ' || nickname[i] == ',' || nickname[i] == '*' || nickname[i] == '?' || nickname[i] == '!' || nickname[i] == '@')
			return (std::cout << "Error: Nickname invalid." << std::endl, ERR_ERRONEUSNICKNAME);
	}
	return (0);
}

std::string Client::toLowercase(const std::string& str)
{
	std::string result = str;
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
	return (result);
}

/* bool Server::isNicknameAvailable(const std::string& nickname, const Client* excludeClient) const
{
    std::string lowerNick = nickname;
    toLowercase(lowerNick);
    
    for (std::vector<Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        const Client* client = *it;
        if (client && client != excludeClient)
        {
            std::string clientNick = client->getNickname();
            toLowercase(clientNick);
            if (clientNick == lowerNick)
                return (false);
        }
    }
    return (true);
}

bool Server::isNicknameAvailable(const std::string& nickname) const
{
    return (isNicknameAvailable(nickname, NULL));
}

void Server::handleNickCommand(Client* client, const std::string& newNickname)
{
    // First check format using Client's validation
    if (client->isNickFormatValid(newNickname) != 0)
    {
        // Send format error to client
        return ;
    }
    
    // Then check uniqueness using Server's validation
    if (!isNicknameAvailable(newNickname, client))
    {
        // Send ERR_NICKNAMEINUSE (433) to client
        return ;
    }
    
    // Nickname is valid and available
    client->setNick(newNickname);
} */

std::string Client::truncName(const std::string& name)
{
	if (name.length() > USERLEN)
		return name.substr(0, USERLEN);
	else
		return (name);
}

int		Client::isUserValid(std::string& userName)
{
	if (userName.size() == 0)
		return (std::cout << /* <client> */ "<USER> :Not enough parameters" << std::endl, ERR_NEEDMOREPARAMS);
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

void	Client::setNick(const std::string& nickName)
{
	if (isNickValid(nickName) == 0)
		_nickname = nickName;
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

Client::Client() {}

Client::~Client() {}

Client::Client(const Client& other)
{
	_nickName = other._nickName;
	_realName = other._realName;
}

Client& Client::operator=(const Client& copy)
{
	if (this != &copy)
	{
		_nickName = copy._nickName;
		_realName = copy._realName;
	}
	return (*this);
}