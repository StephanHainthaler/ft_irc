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
#include <cstring>
#include <cstdlib>

#include "../headers/Server.hpp"
#include "../headers/Parser.hpp"
#include "../headers/Client.hpp"
#include "../headers/Channel.hpp"
#include "../headers/main.hpp"


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
	return (true);
}

/* HowTo start the chat:
on 1st Terminal run: ./ircserv <port> <password>
	make re && ./ircserv "6667" "password" 
on 2nd Terminal run: 
	flatpak run io.github.Hexchat
*/
int	main(int argc, char *argv[])
{
	if (argc != 3)
		return (std::cerr << RED << "Error: incorrect number of arguments" << DEFAULT << std::endl, 1);
	if (isPositiveNumber(argv[1]) == false)
		return (std::cerr << RED << "Error: port MUST be a positive number" << DEFAULT << std::endl, 1);
	
	unsigned int	port = atoi(argv[1]);
	std::string		password = argv[2];

	//handleInput();
	
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

// Client main

#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring> // BUGFIX: allowed?

#include "../headers/Server.hpp"
#include "../headers/Parser.hpp"
#include "../headers/Client.hpp"

//NAME CHECKS

/* int main(void)
{
	Client client;

	client.setNick("julianwadawd");
	if (client.connectToServer("127.0.0.1", 6667) == -1)
	{
		std::cerr << "Failed to connect to server" << std::endl;
		return (1);
	}

	client.setNick("julian");
	if (!client.getNickname().empty())
		std::cout << "Nick: " << client.getNickname() << std::endl;

	std::string userName = "Julian";
    std::string realName = "Julian Uitz";

	client.setUser(userName, 0, '*', realName);
		std::cout << "User: " << client.getUsername() << "\n" << "Real name: " << client.getRealname() << std::endl;

	return (0);
} */

//MODE CHECKS

/* int main(void)
{
	Client client;
	client.setMode('o', true);
	client.setMode('w', true);

	std::cout << "Current modes: " << client.getModes() << std::endl;

	//client.setMode('o', false);
	client.setMode('w', false);

	if (client.hasMode('o'))
		std::cout << "Client has mode" << std::endl;

	std::cout << "Current modes: " << client.getModes() << std::endl;
} */
