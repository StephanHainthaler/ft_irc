/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 09:22:12 by codespace         #+#    #+#             */
/*   Updated: 2025/07/19 14:15:52 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Client.hpp"
#include "../headers/main.hpp"

int Client::connectToServer(const std::string& serverIP, int serverPort)
{
    // Create socket
    _socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (_socketFD == -1)
    {
        std::cerr << "Error: Failed to create socket" << std::endl;
        return (-1);
    }
    
    // Set up server address
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    
    // Convert IP address (with error checking)
    int errorCheck = 0;
	errorCheck = inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);
    if (errorCheck == 0)
    {
        std::cerr << "Error: Invalid IP address format: " << serverIP << std::endl;
        close(_socketFD);
        return (-1);
    }
    else if (errorCheck == -1)
    {
        std::cerr << "Error: System error in inet_pton" << std::endl;
        close(_socketFD);
        return (-1);
    }
    
    // Connect to server
    if (connect(_socketFD, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cerr << "Error: Failed to connect to server " << serverIP << ":" << serverPort << std::endl;
        close(_socketFD);
        return (-1);
    }
    
    _IP = serverIP;
    _port = serverPort;
    setState(CONNECTING);
    
    std::cout << "Connected to server " << serverIP << ":" << serverPort << std::endl;
    return (_socketFD);
}


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