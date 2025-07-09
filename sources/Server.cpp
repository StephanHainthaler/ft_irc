/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 15:52:56 by shaintha          #+#    #+#             */
/*   Updated: 2025/07/09 15:52:56 by shaintha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Server.hpp"

Server::Server(const unsigned int &port, const std::string &password): port(port), password(password)
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);  // Create a TCP/IPv4 socket
    if (socket_fd == -1) 
    {
        std::cerr << "Error. Failed to create server socket." << std::endl;
        return 1;
    }
    
    memset(&sin, 0, sizeof(sin)); // recommended to not store nonsense
    sin.sin_family = AF_INET; // set the address family to IPv4 addresse
    sin.sin_addr.s_addr = INADDR_ANY; // server should accept connections from any IP address
    sin.sin_port = htons(port); // defines the port number the socket will use to communicate on server side (the value has to be converted from host byte order to network byte order)

}



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

	...
	close(sockfd); // Close the server socket when done
	std::cout << GRAY << "Server socket closed." << DEFAULT << std::endl;