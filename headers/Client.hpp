/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juitz <juitz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 13:53:54 by juitz             #+#    #+#             */
/*   Updated: 2025/07/11 12:13:24 by juitz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <cstddef>
#include <iostream>
#include <string>
#include <netinet/in.h>
#include "main.hpp"
#include <vector>

#define MAX_CHANNELS 10
class Client
{
	private:
		size_t		_port;
		size_t		_socketFD;
		std::string	_IP;
		std::string _userName;
		std::string _nickName;
		std::string _realName;
		ClientState _state;
		std::vector<std::string> _channels;
	
	public:
		void	isNickValid(const std::string& nickName);

		//USER_function
		//NICK_function
		//JOIN_function
		
		Client(int socketFD, const sockaddr_in& clientAddr);
		Client();
		~Client();
		
		// Client(const Client &other);
		// Client& operator=(const Client &copy);

		class NickNameTooLong : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
		class NickNameInvalid: public std::exception
		{
			public:
				virtual const char* what() const throw();
		};

		void setState(ClientState newState);
		ClientState getState() const;
};