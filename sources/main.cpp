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
	
	unsigned int	port = std::atoi(argv[1]);
	std::string		password = argv[2];

	(void)port;
	(void)password;

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
