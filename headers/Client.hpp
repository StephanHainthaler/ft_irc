/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shaintha <shaintha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 13:53:54 by juitz             #+#    #+#             */
/*   Updated: 2025/08/07 09:47:46 by shaintha         ###   ########.fr       */
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

class Channel;

class Client
{
	private:
		int			_socketFD;
		int			_port;
		std::string	_IP;
		std::string _nickname;
		std::string _userName;
		std::string _hostname;
		std::string _realName;
		
		//std::string _messageBuffer;
		std::string _modes;
		ClientState _state;
		std::vector<std::string> _channels;

		static	std::string toLowercase(const std::string& str);
        static	std::string truncName(const std::string& name);
		
	public:

		// Format checks
		bool	isNickValid(const std::string& nickname) const;
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
		std::vector<std::string>	getChannels() const;
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
