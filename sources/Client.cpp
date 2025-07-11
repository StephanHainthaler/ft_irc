/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juitz <juitz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 13:54:03 by juitz             #+#    #+#             */
/*   Updated: 2025/07/11 14:52:24 by juitz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Client.hpp"
#include <netinet/in.h>

const char* Client::NickNameTooLong::what() const throw()
{
   return ("Error: Nickname can only");
}

const char* Client::NickNameInvalid::what() const throw()
{
   return ("Error: Nickname invalid ");
}

void	Client::isNickValid(const std::string& nickName)
{
	for (size_t i = 0; i < nickName.size(); i++)
	{
		if (nickName.size() > 9)
			throw NickNameTooLong();
		if (nickName[i] == ' ' || nickName[i] == ',' || nickName[i] == '*' || nickName[i] == '?' || nickName[i] == '!' || nickName[i] == '@')
			throw NickNameInvalid();
		if (nickName[0] == '$' || nickName[0] == ':' || nickName[0] == '#' || nickName[0] == '~' || nickName[0] == '&' || nickName[0] == '+')
			throw NickNameInvalid();
	}
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