/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shaintha <shaintha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 15:53:47 by shaintha          #+#    #+#             */
/*   Updated: 2025/07/09 15:53:47 by shaintha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <sys/socket.h> // for socket, bind, listen, accept
# include <netinet/in.h> // for sockaddr_in
# include <fcntl.h> // to set socket to non-blocking mode

# include <unistd.h>
# include <exception>
# include <vector>
# include <poll.h>

# include "main.hpp"
# include "Client.hpp"
# include "Channel.hpp"

# define MAX_CLIENTS 10 // max #clients that can connect to the server at the same time

// Server states
# define RUNNING 1
# define NOT_RUNNING 0
# define ERROR -1

// useful: https:://www.geeksforgeeks.org/cpp/socket-programming-in-cpp/
class Server
{
	public:
		Server(const unsigned int &port, const std::string &password);
		~Server(void);

		// Getters
		std::string getPassword(void) const;
		// Channel *getChannel(const std::string &channel_name) const;
		sockaddr_in getServerAddress(void) const; // bc client will need it to connect to server
		int getState(void) const;

		// Member functions - server actions
		void sendMessageToIRCClient(const char* msg);
		void handleClientConnections(void);

		// Member functions - user triggered actions
		void addClient(Client *client);
		void removeClient(Client *client);
		
		/*
		void addChannel(Channel *channel);
		void removeChannel(Channel *channel);

		// Nickname availability checks
		void toLowercase(const std::string& str);
		bool isNicknameAvailable(const std::string& nickname, const Client* excludeClient) const;
		bool isNicknameAvailable(const std::string& nickname) const;
		void handleNickCommand(Client* client, const std::string& newNickname);
		
		// Exception
		class ServerException: public std::exception
		{
			public:
				ServerException(const std::string &message);
				virtual const char* what() const throw();
				virtual ~ServerException() throw();

			private:
				const std::string _message;
		};

	private:
		Server(void);
		Server(const Server &other);
		Server	&operator=(const Server &other);
	
		int							_server_fd; // Server Socket FD - Listening socket, can be negative
		const unsigned int 			_port; // Port number - "door to the server"
		struct sockaddr_in 			_serverAddress; // for IPv4 - holds network info - like IP address and port number - that the server uses to know where to listen or connect
		// struct sockaddr_in6 		_serverAddress; // for IPv6 - holds network info - like IP address and port number - that the server uses to know where to listen or connect
		/*
		server creates sockaddr_in "serverAddress" to specify its own IP address and port to bind to
		client takes this sockaddr_in "serverAddress" to specify the server's IP address and port to connect to
		*/
		
		const std::string			_password;
		std::vector<Client *>		_clients;	// List of connected clients (ClientClass objs)
		//std::vector<Channel *>		_channels;	// List of channels (ChannelClass objs)
		//std::vector<std::string>	_users; // auf 10 users limitieren

		// clients must be unique within a channel

		int							_state; // Server state - 0: not running, 1: running, -1: error (?)
		int							_IRC_client_fd; // FD of the IRC client (Hexchat) that connects to the server
};

/* 
***USED FUNCTIONS:
socket(AF_INET, SOCK_STREAM, 0)
close(fd)
connect(sockfd, (sockaddr*)&addr, sizeof(addr))
bind(sockfd, (sockaddr*)&addr, sizeof(addr))
listen(sockfd, SOMAXCONN)
accept(sockfd, NULL, NULL)
htons()
fcntl(sockfd, F_SETFL, O_NONBLOCK)
send(client_fd, buffer, len, 0)
recv(client_fd, buffer, sizeof(buffer), 0)

***MIGHT BE USEFUL:
setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))
lseek(fd, 0, SEEK_END)

***UNUSED FUNCTIONS:
getsockname(sockfd, (sockaddr*)&addr, &len)getprotobyname("tcp")
gethostbyname("irc.example.com")
getaddrinfo("localhost", "6667", &hints, &res)
freeaddrinfo()
ntohs()
htonl()
ntohl(addr.sin_addr.s_addr)
inet_addr("127.0.0.1")
inet_ntoa(addr.sin_addr)
signal(SIGINT, handle_sigint)
sigaction(SIGINT, &sa, NULL)
fstat(fd, &info)
poll(fds, nfds, timeout)
*/