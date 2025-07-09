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

class Server
{
	public:
		Server(const unsigned int &port, const std::string &password);
		~Server(void);
		
		void bindServerSocket(void);
		void listenServerSocket(void);
		void acceptClientConnections(void);
		
		void handleClientMessage(int client_fd);

	private:
		Server(void);
		Server(const Server &other);
		Server	&operator=(const Server &other);

		
		const unsigned int	socket_fd; // Server Socket FD - Listening socket (IP + port combo)
		// IP
		const unsigned int port;		
		struct sockaddr_in sin; // holds network info - like IP address and port number - that the server uses to know where to listen or connect
		
		const std::string	password;
			// List of connected clients (ClientClass objs)
			// List of channels (ChannelClass objs)

};

#endif
