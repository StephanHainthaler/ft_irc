/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shaintha <shaintha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 23:08:03 by shaintha          #+#    #+#             */
/*   Updated: 2025/07/09 23:08:03 by shaintha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Parser.hpp"

void	handleInput(void)
{
	std::string					input;
	std::vector<std::string>	command;

	while (std::cin)
	{
		std::cout << "TYPE the COMMAND" << std::endl;
		getline(std::cin, input);
		parseInputToVector(input, &command);
		// printVector(&command);
		executeCommand(command);
		command.clear();
	}
}

void    parseInputToVector(std::string &input, std::vector<std::string> *command)
{
    const char* delimiters = " \f\n\r\t\v";
	for (char* token = std::strtok((char *)input.c_str(), delimiters); token; token = std::strtok(NULL, delimiters))
		command->push_back(token);
}

void	executeCommand(std::vector<std::string> command)
{
	if (command.size() == 0)
		return ;
	else if (command[0].compare("AUTHENTICATE") == 0)
		std::cout << "AUTHENTICATE" << std::endl;
	else if (command[0].compare("NICK") == 0)
		std::cout << "NICK" << std::endl;
	else if (command[0].compare("USER") == 0)
		std::cout << "USER" << std::endl;
	else if (command[0].compare("JOIN") == 0)
		std::cout << "JOIN" << std::endl;
	else if (command[0].compare("PRIVMSG") == 0)
		std::cout << "PRIVMSG" << std::endl; // (is the same for recieving?)
	else if (command[0].compare("KICK") == 0)
		std::cout << "KICK" << std::endl;
	else if (command[0].compare("INVITE") == 0)
		std::cout << "INVITE" << std::endl;
	else if (command[0].compare("TOPIC") == 0)
		std::cout << "TOPIC" << std::endl;
	else if (command[0].compare("MODE") == 0)
		std::cout << "MODE" << std::endl; // -i -t -k -o -l
	else
		std::cerr << "Command NOT found" << std::endl;
}

void printVector(std::vector<std::string> vector)
{
	if (vector.size() == 0)
	{
		std::cout << "EMPTY VECTOR!!" << std::endl;
		return ;
	}
	for (size_t i = 0; i < vector.size(); i++)
	{			
		std::cout << vector[i] << std::endl;
	}
}
