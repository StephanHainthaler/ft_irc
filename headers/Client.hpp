/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 13:53:54 by juitz             #+#    #+#             */
/*   Updated: 2025/07/17 10:46:15 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
//#include "Server.hpp"
#include "main.hpp"
#include <cstddef>
#include <iostream>
#include <string>
#include <netinet/in.h>
#include <vector>
#include <algorithm>
#include <cctype>

#define CHANLIMIT 10
#define USERLEN 15

// Errors
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
		std::string _nickname;
		std::string _realName;
		ClientState _state;
		std::vector<std::string> _channels;

		static	std::string toLowercase(const std::string& str);
        static	std::string truncName(const std::string& name);
		
	public:
		int		isNickValid(const std::string& nickname) const;
		bool	isRealNameValid(const std::string& realName) const;
		int		isUserValid(std::string& userName);
		bool	isNicknameAvailable(const std::string& nickname) const;
        bool	isNicknameAvailable(const std::string& nickname, const Client* excludeClient) const;
		void	setNick(const std::string& nickname);
		void	setUser(std::string& userName, int zero, char asterisk, std::string& realName); // 2nd parameter should always be zero and 3rd "*"
		void	setState(ClientState newState);

		// Getters
		std::string getNickname() const;
		std::string getUsername() const;
		std::string getRealname() const;
		ClientState getState() const;

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

};