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

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
#include <sys/socket.h> // for socket, bind, listen, accept
#include <netinet/in.h> // for sockaddr_in

#include <exception>
#include <vector>

#include "Client.hpp"
#include "Channel.hpp"

class Server // useful: https:://www.geeksforgeeks.org/cpp/socket-programming-in-cpp/
{
	public:
		Server(const unsigned int &port, const std::string &password);
		~Server(void);
		
		// Getters
		std::string get_password(void) const;
		Channel *get_channel(const std::string &channel_name) const;
		

		// Member functions - server actions
		void makeListen(void);
		void acceptClientConnection(Client *client);
		void handleClientMessage(int client_fd);
		

		// Member functions - user triggered actions
		void addClient(Client *client);
		void removeClient(Client *client);
		
		void addChannel(Channel *channel);
		void removeChannel(Channel *channel);
		

		class ServerException: public std::exception
		{
			public:
				ServerException(std::string message) : _message(message) {}
				const char* what() const throw();
			
			private :
				std::string _message;
		};

	private:
		Server(void);
		Server(const Server &other);
		Server	&operator=(const Server &other);
	
		
		int						_socket_fd; // Server Socket FD - Listening socket, can be negative
		const unsigned int 		_port; // Port number - "door to the server"
		struct sockaddr_in 		_serverAddress; // for IPv4 - holds network info - like IP address and port number - that the server uses to know where to listen or connect
		// struct sockaddr_in6 	_serverAddress; // for IPv6 - holds network info - like IP address and port number - that the server uses to know where to listen or connect
		/*
		server creates sockaddr_in "serverAddress" to specify its own IP address and port to bind to
		client takes this sockaddr_in "serverAddress" to specify the server's IP address and port to connect to
		*/

		const std::string		_password;
		std::vector<Client *>	_clients;	// List of connected clients (ClientClass objs)
		std::vector<Channel *>	_channels;	// List of channels (ChannelClass objs)

};

#endif

/* 
***USED FUNCTIONS:
socket(AF_INET, SOCK_STREAM, 0)close(fd)
connect(sockfd, (sockaddr*)&addr, sizeof(addr))
bind(sockfd, (sockaddr*)&addr, sizeof(addr))
listen(sockfd, SOMAXCONN)
accept(sockfd, NULL, NULL)
htons()
fcntl(sockfd, F_SETFL, O_NONBLOCK)

***MIGHT BE USEFUL:
setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))
recv(client_fd, buffer, sizeof(buffer), 0)
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
send(client_fd, buffer, len, 0)
signal(SIGINT, handle_sigint)
sigaction(SIGINT, &sa, NULL)
fstat(fd, &info)
poll(fds, nfds, timeout)
*/