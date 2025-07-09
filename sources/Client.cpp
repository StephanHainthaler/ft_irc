/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juitz <juitz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 13:54:03 by juitz             #+#    #+#             */
/*   Updated: 2025/07/09 14:32:38 by juitz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

int Client::isNickValid(const std::string& nickName)
{
	for (size_t i = 0; i < nickName.size(); i++)
	{
		if (nickName[i] == ' ' || nickName[i] == ',' || nickName[i] == '*' || nickName[i] == '?' || nickName[i] == '!' || nickName[i] == '@')
			return (1);
		if (nickName.size() > 9)
			return (1);
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