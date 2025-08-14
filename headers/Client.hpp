/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 09:21:05 by juitz             #+#    #+#             */
/*   Updated: 2025/08/14 17:13:14 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Server.hpp"

# define CHANLIMIT 10
# define USERLEN 15

enum ClientState
{
	CONNECTING,
	AUTHENTICATED,
	REGISTERED,
	DISCONNECTED
};

class Client
{	
	public:
		Client(void);
		Client(int socketFD, int port);
		~Client(void);

		// Setters & Getters
		std::string		getClientName(void) const;
		void			setNickname(const std::string &nickname);
		std::string 	getNickname(void) const;
		void			setUsername(const std::string &userName);
		std::string 	getUsername(void) const;
		void			setHostname(const std::string &hostname);
		std::string		getHostname(void) const;
		void			setRealname(const std::string &realName);
		std::string 	getRealname(void) const;
		void			setSocketFD(int socketFD);
		int				getSocketFD() const;
		void			setIP(const std::string &ip);
		std::string 	getIP(void) const;
		void			setState(ClientState newState);
		ClientState 	getState(void) const;
		void			setChannelNumber(int number);
		size_t			getChannelNumber(void) const;

		bool			isNicknameValid(const std::string& nickname) const;
		bool			isUsernameValid(const std::string& userName);
		bool			isRealnameValid(const std::string& realName) const;
		void 			disconnect(void);

	private:
		std::string		_nickname;
		std::string		_userName;
		std::string 	_hostname;
		std::string		_realName;
		int				_socketFD;
		int				_port;
		std::string		_IP;
		ClientState 	_state;
		size_t			_numberOfChannels;

};

#endif
