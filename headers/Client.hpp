/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juitz <juitz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2025/08/12 15:50:40 by juitz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

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

enum ClientState
{
	CONNECTING,
	AUTHENTICATED,
	REGISTERED,
	DISCONNECTED
};

class Channel;

class Client
{
	private:
		int							_socketFD;
		int							_port;
		std::string					_IP;
		std::string					_nickname;
		std::string					_userName;
		std::string 				_hostname;
		std::string					_realName;
		std::string					_modes;
		ClientState 				_state;
		size_t						_numberOfChannels;

		static	std::string truncName(const std::string& name);
		
	public:

		// Format checks
		bool		isNickValid(const std::string& nickname) const;
		bool		isRealNameValid(const std::string& realName) const;
		int			isUserValid(std::string& userName);
		std::string getFullIdentifier() const;
		
		// Connection
		void 						disconnect(void);

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
		void	setChannelNumber(int number);
	

		// Getters
		std::string 				getNickname(void) const;
		std::string 				getUsername(void) const;
		std::string 				getRealname(void) const;
		ClientState 				getState(void) const;
		int							getSocketFD() const;
		std::string 				getIP(void) const;
		std::string					getHostname(void) const;
		size_t						getChannelNumber() const;
		std::string 				getModes(void) const;
		

		//ADDED BY STEPHAN
		std::string					getClientName(void) const;
	
		
		Client(int socketFD, int port);
		Client(void);
		~Client(void);
		
		// Client(const Client &other);
		// Client& operator=(const Client &copy);
};
