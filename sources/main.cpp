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
	hexchat
*/
int	main(int argc, char *argv[])
{
	if (argc != 3)
		return (std::cerr << RED << "Error: incorrect number of arguments" << DEFAULT << std::endl, 1);
	if (isPositiveNumber(argv[1]) == false)
		return (std::cerr << RED << "Error: port MUST be a positive number" << DEFAULT << std::endl, 1);
	
	std::string		password = argv[2];
	
	try
	{
		Server	server(atoi(argv[1]), password);

		server.run(); // start the server and listen for incoming connections
	}
	catch (const std::exception& e)
	{
		std::cerr << RED << e.what() << DEFAULT << std::endl;
	}
	return (0);
}

// bool	isDelimiter(char c, const char *delimiters)
// {
// 	for (size_t i = 0; i < std::strlen(delimiters); i++)
// 		if (c == delimiters[i])
// 			return (true);
// 	return (false);
// }

// std::vector<std::string>    parseStringToVector(std::string &input, const char *delimiters)
// {
// 	std::vector<std::string>	vector;
// 	size_t i = 0;

// 	std::cout << "String to Parse: \n" << input << std::endl;

// 	for (char* token = std::strtok((char *)input.c_str(), delimiters); token; token = std::strtok(NULL, delimiters))
// 	{
// 		std::cout << input << std::endl;
// 		vector.push_back(token);
// 		i += std::strlen(token) - 1;
// 		std::cout << "Charcter: '" << input[i] << "' is at " << i << std::endl;

// 		for (size_t pos = i; isDelimiter(input[pos], ",") == true; pos++)
// 		{
// 			std::cout << input[pos] << std::endl;
// 		}
// 	}
// 	return (vector);
		
// }

// int	main(int argc, char *argv[])
// {
// 	if (argc == 1)
// 		return (1);
	
// 	std::vector<std::string>	args;
// 	std::string test = argv[1];

// 	args = parseStringToVector(test, ",");
// 	for (size_t i = 0; i < args.size(); i++)
// 	{
// 		std::cout << args[i] << std::endl;
// 	}

// 	return (0);
// }
