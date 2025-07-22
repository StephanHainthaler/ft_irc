/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juitz <juitz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 13:53:54 by juitz             #+#    #+#             */
/*   Updated: 2025/07/22 17:08:29 by juitz            ###   ########.fr       */
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
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define CHANLIMIT 10
#define USERLEN 15

//Numerics
#define RPL_WELCOME 001

// Numeric Errors
#define ERR_NOSUCHNICK 401
#define ERR_NOSUCHCHANNEL 403
#define ERR_TOOMANYCHANNELS 405
#define ERR_INPUTTOOLONG 417
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE 433
#define ERR_NEEDMOREPARAMS 461
#define ERR_ALREADYREGISTERED 462 

class Client
{
	private:
		int		_socketFD;
		int		_port;
		std::string	_IP;
		std::string _userName;
		std::string _nickname;
		std::string _realName;
		std::string _hostname;
		std::string _messageBuffer;
		ClientState _state;
		std::vector<std::string> _channels;

		static	std::string toLowercase(const std::string& str);
        static	std::string truncName(const std::string& name);
		
	public:

		// Format checks
		int		isNickValid(const std::string& nickname) const;
		bool	isRealNameValid(const std::string& realName) const;
		int		isUserValid(std::string& userName);
		//bool	isNicknameAvailable(const std::string& nickname) const;
        //bool	isNicknameAvailable(const std::string& nickname, const Client* excludeClient) const;
		
		// Setters
		void	setNick(const std::string& nickname);
		void	setUser(std::string& userName, int zero, char asterisk, std::string& realName); // 2nd parameter should always be zero and 3rd "*"
		void	setState(ClientState newState);

		// Getters
		std::string getNickname() const;
		std::string getUsername() const;
		std::string getRealname() const;
		ClientState getState() const;
		int			getSocketFD() const;
		
		// Connection
		int 						connectToServer(const std::string& serverIP, int serverPort);
		int 						sendMessage(const std::string& message);
		std::vector<std::string> 	receiveCompleteMessages();
		void 						disconnect();

		// States
		void	isFullyRegistered();
		

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
