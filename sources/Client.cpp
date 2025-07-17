/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 09:22:12 by codespace         #+#    #+#             */
/*   Updated: 2025/07/17 09:25:38 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// const char* Client::NickNameTooLong::what() const throw()
// {
//    return ("Error: Nickname can only be max 9 characters long");
// }

// const char* Client::NickNameInvalid::what() const throw()
// {
//    return ("Error: Nickname invalid ");
// }

int	Client::isNickValid(const std::string& nickname) const
{
	for (size_t i = 0; i < nickName.size(); i++)
	{
		if (nickName.size() > 9 || nickName.size() == 0)
			return (std::cout << "Error: Nickname must be at least 1 character and can only be max 9 characters long." << std::endl, ERR_ERRONEUSNICKNAME);
		if (nickName[0] == '$' || nickName[0] == ':' || nickName[0] == '#' || nickName[0] == '~' || nickName[0] == '&' || nickName[0] == '+')
			return (std::cout << "Error: Nickname invalid." << std::endl, ERR_ERRONEUSNICKNAME);
		if (nickName[i] == ' ' || nickName[i] == ',' || nickName[i] == '*' || nickName[i] == '?' || nickName[i] == '!' || nickName[i] == '@')
			return (std::cout << "Error: Nickname invalid." << std::endl, ERR_ERRONEUSNICKNAME);
	}
	return (0);
}

void toLowercase(const std::string& str)
{
	std::string result = str;
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
	return (result);
}

bool Server::isNicknameAvailable(const std::string& nickname, const Client* excludeClient) const
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
}

std::string truncName(std::string name)
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

bool	Client::isRealNameValid(const std::string& realName)
{
	
}

void	Client::setNick(const std::string& nickName)
{
	if (isNickValid(nickName))
		_nickName = nickName;
}

void Client::setUser(std::string& userName, int zero, char *, std::string& realName)
{
	if (isUserValid(userName))
		_userName = userName;
}

Client::Client(int socketFD, const sockaddr_in& clientAddr) : 
	_socketFD(socketFD),
	_state(CONNECTING),
	_channels(MAX_CHANNELS)
{
}
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
	return (_nickName);
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

// Client::Client(const Client& other)
// {
// 	_nickName = other._nickName;
// 	_realName = other._realName;
// }

// Client& Client::operator=(const Client& copy)
// {
// 	if (this != &copy)
// 	{
// 		_nickName = copy._nickName;
// 		_realName = copy._realName;
// 	}
// 	return (*this);
// }