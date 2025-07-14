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

#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring> // BUGFIX: allowed?

#include "../headers/Server.hpp"

bool	isPositiveNumber(char *string)
{
	size_t	i = 0, end = std::strlen(string);

	if (string[i] == '\0' || string == NULL)
		return (false);
	while (isspace(string[i]) == true)
		i++;
	if (string[i] == '-')
		return (false);
	if (string[i] == '+')
		i++;
	if (!(string[i] >= '0' && string[i] <= '9'))
		return (false);
	for ( ; (string[i] >= '0' && string[i] <= '9'); i++)
	{
	}
	if (i != end)
		return (false);
	if (atoi(string) <= 0 || atoi(string) > 65535) // https://www.pico.net/kb/what-is-the-highest-tcp-port-number-allowed/
		return (false);
	return (true);
}

// ./ircserv <port> <password>
// c++ main.cpp -o ircserv -std=c++98 && ./ircserv "8080" "password"
// make re && ./ircserv "8080" "password"
int	main(int argc, char *argv[])
{
	if (argc != 3)
		return (std::cerr << RED << "Error: incorrect number of arguments" << DEFAULT << std::endl, 1);
	if (isPositiveNumber(argv[1]) == false)
		return (std::cerr << RED << "Error: port MUST be a positive number between 0 and 65535" << DEFAULT << std::endl, 1);
	
	unsigned int 	port = atoi(argv[1]);
	std::string		password = argv[2];

	try
	{
		Server	server(port, password);
		server.run(); // start the server and listen for incoming connections
	}
	catch(const std::exception& e)
	{
		std::cerr << RED << e.what() << DEFAULT << std::endl;
	}

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