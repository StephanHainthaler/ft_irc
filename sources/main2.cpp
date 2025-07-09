/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shaintha <shaintha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 16:30:15 by shaintha          #+#    #+#             */
/*   Updated: 2025/07/01 16:33:08 by shaintha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# define DEFAULT "\x1b[0m"
# define RED "\x1b[31m"
# define YELLOW "\x1b[33m"
# define GRAY "\x1b[90m"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

// ./ircserv <port> <password>
// c++ main.cpp -o ircserv -std=c++98 && ./ircserv "8080" "password"
// make re && ./ircserv "8080" "password"
int	main(int argc, char *argv[])
{
	if (argc != 3)
		return (std::cerr << RED << "Error: incorrect number of arguments" << DEFAULT << std::endl, 1);
	
	int port = std::atoi(argv[1]);

	if (port <= 0 || port > 65535) // https://www.pico.net/kb/what-is-the-highest-tcp-port-number-allowed/
		return (std::cerr << RED << "Error: invalid port number" << DEFAULT << std::endl, 1);
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);  // Create a TCP/IPv4 socket
    if (sockfd == -1) 
    {
        std::cerr << "Error. Failed to create socket." << std::endl;
        return 1;
    }

	struct sockaddr_in sin; // this codepart can also be found in srv_create.c
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET; // IPv4
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port); // Converts a 16-bit value (short) from host byte order to network byte order
    
	if (bind(sockfd, (struct sockaddr *)&sin, sizeof(sin)) == -1) // Binds a socket to an IP and port and must be done before listening
	{
		std::cerr << "Error. Failed to bind socket." << std::endl;
		close(sockfd);
		return 1;
	}

	if (listen(sockfd, SOMAXCONN) == -1) // Marks a bound socket as passive, meaning it will be used to accept incoming connection requests
	{
		std::cerr << "Error. Failed to listen on socket." << std::endl;
		close(sockfd);
		return 1;
	}

	std::cout << GRAY << "Server is listening on port " << port << DEFAULT << std::endl;

	while (true) 
	{
		int client_fd = accept(sockfd, NULL, NULL); // Accepts an incoming connection on a listening socket
		// accept blocks unless the listening socket is set to non-blocking mode
		if (client_fd == -1) 
		{
			std::cerr << "Error. Failed to accept connection." << std::endl;
			close(sockfd);
			return 1;
		}

		std::cout << YELLOW << "New client connected." << DEFAULT << std::endl;

		const char *welcome_msg = "Welcome to the IRC server!\n";
		ssize_t total_sent = 0;
		ssize_t msg_len = strlen(welcome_msg);

		while (total_sent < msg_len) 
		{
			ssize_t sent = send(client_fd, welcome_msg + total_sent, msg_len - total_sent, 0);
			if (sent == -1) 
				break;
			total_sent += sent;
		}

		close(client_fd); // Close the client socket after sending the message
	}
	close(sockfd); // Close the server socket when done
	std::cout << GRAY << "Server socket closed." << DEFAULT << std::endl;
	return 0;
}

/* Socket vs Server
The difference between a socket and a server is that a socket is an endpoint for communication,
while a server is a program that listens for incoming connections on a specific port 
and can handle multiple clients. 
A server uses sockets to accept connections from clients, 
allowing them to communicate over the network.
A server can have multiple sockets, each representing a different client connection, 
while a socket is a single communication endpoint.
*/