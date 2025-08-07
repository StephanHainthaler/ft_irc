/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 13:53:54 by juitz             #+#    #+#             */
/*   Updated: 2025/08/06 00:28:32 by marvin           ###   ########.fr       */
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

// //Numerics
// #define RPL_WELCOME2 001

// // Numeric Errors
// #define ERR_NOSUCHNICK2 401
// #define ERR_NOSUCHCHANNEL2 403
// #define ERR_TOOMANYCHANNELS2 405
// #define ERR_INPUTTOOLONG2 417
// #define ERR_NONICKNAMEGIVEN2 431
// #define ERR_ERRONEUSNICKNAME2 432
// #define ERR_NICKNAMEINUSE2 433
// #define ERR_NEEDMOREPARAMS2 461
// #define ERR_ALREADYREGISTERED2 462
// #define ERR_UNKNOWNMODE2 472

// //ADDED BY STEPHAN
// #define RPL_CHANNELMODEIS2 324
// #define RPL_NOTOPIC2 331
// #define RPL_TOPIC2 332
// #define RPL_INVITING2 341
// #define RPL_NAMREPLY2 353
// #define RPL_ENDOFNAMES2 366
// #define ERR_UNKNOWNCOMMAND2 421
// #define ERR_USERNOTINCHANNEL2 441
// #define ERR_NOTONCHANNEL2 442
// #define ERR_USERONCHANNEL2 443
// #define ERR_ALREADYREGISTERED2 462
// #define ERR_PASSWDMISMATCH2 464
// #define ERR_CHANNELISFULL2 471
// #define ERR_INVITEONLYCHAN2 473
// #define ERR_BANNEDFROMCHAN2 474
// #define ERR_BADCHANNELKEY2 475
// #define ERR_BADCHANMASK2 476
// #define ERR_CHANOPRIVSNEEDED2 482







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

		static	std::string truncName(const std::string& name);
		
	public:

		// Format checks
		bool	isNickValid(const std::string& nickname) const;
		bool	isRealNameValid(const std::string& realName) const;
		int		isUserValid(std::string& userName);
		std::string getFullIdentifier() const;
		
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
		

		//ADDED BY STEPHAN
		std::string					getClientName(void) const;
	
		//USER_function
		//NICK_function
		//JOIN_function
		
		Client(int socketFD, int port);
		Client();
		~Client();
		
		// Client(const Client &other);
		// Client& operator=(const Client &copy);
};
