/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shaintha <shaintha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 13:53:54 by juitz             #+#    #+#             */
/*   Updated: 2025/08/04 11:02:18 by shaintha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "main.hpp"
#include "Server.hpp"
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
#include <algorithm>

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
#define ERR_UNKNOWNMODE 472

//ADDED BY STEPHAN
#define RPL_CHANNELMODEIS 324
#define RPL_NOTOPIC 331
#define RPL_TOPIC 332
#define RPL_INVITING 341
#define RPL_NAMREPLY 353
#define RPL_ENDOFNAMES 366
#define ERR_UNKNOWNCOMMAND 421
#define ERR_USERNOTINCHANNEL 441
#define ERR_NOTONCHANNEL 442
#define ERR_USERONCHANNEL 443
#define ERR_ALREADYREGISTERED 462
#define ERR_PASSWDMISMATCH 464
#define ERR_CHANNELISFULL 471
#define ERR_INVITEONLYCHAN 473
#define ERR_BANNEDFROMCHAN 474
#define ERR_BADCHANNELKEY 475
#define ERR_BADCHANMASK 476
#define ERR_CHANOPRIVSNEEDED 482







class Channel;


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
		//std::string _messageBuffer;
		std::string _modes;
		ClientState _state;
		std::vector<std::string> _channels;

		static	std::string toLowercase(const std::string& str);
        static	std::string truncName(const std::string& name);
		
	public:

		// Format checks
		int		isNickValid(const std::string& nickname) const;
		bool	isRealNameValid(const std::string& realName) const;
		int		isUserValid(std::string& userName);
		std::string getFullIdentifier() const;
		//bool	isNicknameAvailable(const std::string& nickname) const;
        //bool	isNicknameAvailable(const std::string& nickname, const Client* excludeClient) const;

		// Connection
		int 						connectToServer(const std::string& serverIP, int serverPort);
		int							sendMessageToChannel(Channel* channel, std::string message);
		std::vector<std::string> 	receiveCompleteMessages();
		void 						disconnect();

		// Channels
		void	joinChannel(const std::string& channelName);
		void	leaveChannel(const std::string& channelName);
		void 	clearChannels();
		
		// Modes
		bool	isValidUserMode(char mode) const;
		bool 	hasMode(char mode) const;

		
		// States
		void	isFullyRegistered();
		
		// Setters
		void	setNick(const std::string& nickname);
		void	setUser(std::string& userName, int zero, char asterisk, std::string& realName); // 2nd parameter should always be zero and 3rd "*"
		void	setState(ClientState newState);
		void	setSocketFD(int socketFD);
		void	setIP(const std::string& ip);
		void	setHostname(const std::string& hostname);
		int 	setMode(char mode, bool enable);

		// Getters
		std::string 				getNickname() const;
		std::string 				getUsername() const;
		std::string 				getRealname() const;
		ClientState 				getState() const;
		int							getSocketFD() const;
		std::string 				getIP() const;
		std::string					getHostname() const;
		std::vector<std::string> 	getChannels() const;
		std::string 				getModes() const;
		
	
		//USER_function
		//NICK_function
		//JOIN_function
		
		Client(int socketFD, int port);
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
