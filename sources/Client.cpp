/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 13:54:03 by juitz             #+#    #+#             */
/*   Updated: 2025/07/14 09:40:08 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Client.hpp"

// const char* Client::NickNameTooLong::what() const throw()
// {
//    return ("Error: Nickname can only be max 9 characters long");
// }

// const char* Client::NickNameInvalid::what() const throw()
// {
//    return ("Error: Nickname invalid ");
// }

bool	Client::isNickValid(const std::string& nickName)
{
	for (size_t i = 0; i < nickName.size(); i++)
	{
		if (nickName.size() > 9 || nickName.size() == 0)
			return (std::cout << "Error: Nickname must be at least 1 character and can only be max 9 characters long.", ERR_ERRONEUSNICKNAME);
		// if nickname in use
			//ERR_NICKNAMEINUSE
		if (nickName[i] == ' ' || nickName[i] == ',' || nickName[i] == '*' || nickName[i] == '?' || nickName[i] == '!' || nickName[i] == '@')
			return (std::cout << "Error: Nickname invalid.", ERR_ERRONEUSNICKNAME);
		if (nickName[0] == '$' || nickName[0] == ':' || nickName[0] == '#' || nickName[0] == '~' || nickName[0] == '&' || nickName[0] == '+')
			return (std::cout << "Error: Nickname invalid.", ERR_ERRONEUSNICKNAME);
	}
	return (true);
}

std::string trunc_name(std::string name)
{
	if (name.length() > USERLEN)
		return name.substr(0, USERLEN);
	else
		return (name);
}

int		Client::isUserValid(const std::string& userName)
{
	for (size_t i = 0; i < userName.size(); i++)
	{
		if (userName.size() == 0)
			return (std::cout << /* <client> */ "<USER> :Not enough parameters", ERR_NEEDMOREPARAMS);
		if (userName.size() > USERLEN)
			trunc_name(userName);
		
	}
}

bool	Client::isRealNameValid(const std::string& realName)
{
	
}

void	Client::setNick(const std::string& nickName)
{
	if (isNickValid(nickName))
		_nickName = nickName;
}

void Client::setUser(const std::string& userName, int zero, char *, const std::string& realName)
{
	
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