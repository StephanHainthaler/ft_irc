/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juitz <juitz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 13:53:54 by juitz             #+#    #+#             */
/*   Updated: 2025/07/11 18:43:52 by juitz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Server.hpp"
#include "main.hpp"
#include <cstddef>
#include <iostream>
#include <string>
#include <netinet/in.h>
#include <vector>

#define MAX_CHANNELS 10
#define USERLEN 15

//errors
#define ERR_NOSUCHNICK 401
#define ERR_NOSUCHCHANNEL 403
#define ERR_TOOMANYCHANNELS 405
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE 433
#define ERR_NEEDMOREPARAMS 461
#define ERR_ALREADYREGISTERED 462 

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
		bool	isNickValid(const std::string& nickName);
		int		isUserValid(const std::string& userName);
		void	setNick(const std::string& nickName);
		void	setUser(const std::string& userName, int zero, char *, const std::string& realName); // 2nd parameter should always be zero and 3rd "*"

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