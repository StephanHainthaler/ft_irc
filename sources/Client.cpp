/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juitz <juitz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 13:54:03 by juitz             #+#    #+#             */
/*   Updated: 2025/07/09 17:45:29 by juitz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Client.hpp"

const char* Client::NickNameTooLong::what() const throw()
{
   return ("Error: Nickname can only");
}

const char* Client::NickNameInvalid::what() const throw()
{
   return ("Error: file is empty ");
}

void	Client::isNickValid(const std::string& nickName)
{
	for (size_t i = 0; i < nickName.size(); i++)
	{
		if (nickName.size() > 9)
			throw NickNameTooLong();
		if (nickName[i] == ' ' || nickName[i] == ',' || nickName[i] == '*' || nickName[i] == '?' || nickName[i] == '!' || nickName[i] == '@')
			throw NickNameInvalid();
		if ()
	
	}
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