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
		parseInputToVector(input, command);
		executeCommand(command);
		command.clear();
	}
}

//DOES NOT WORK PROPERLY YET (AND DONT USE WITH VALGRIND)
void    parseInputToVector(std::string &input, std::vector<std::string> &command)
{
	std::cout << "String to parse: '" << input << "'" << std::endl;

	for (size_t i = 0, j; i < input.length(); i++)
	{
		while (input[i] == ' ' || input[i] == '\f' || input[i] == '\n' || input[i] == '\r' || input[i] == '\t' || input[i] == '\v')
			i++;
		if (i == input.length())
			break ;
		j = i;
		while (i < input.length() && (input[i] != ' ' && input[i] != '\f' && input[i] != '\n' && input[i] != '\r' && input[i] != '\t' && input[i] != '\v'))
			i++;
		std::cout << "Substring: " << input.substr(j, i) << std::endl;
		command.push_back(input.substr(j, i));
	}
	
	std::cout << "TEST: Printing the vector" << std::endl;
	printVector(command);
}


void	executeCommand(std::vector<std::string> command)
{
	//std::cout << "Command = " << command[0] << std::endl; 
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
	for (std::vector<std::string>::iterator it = vector.begin(); it != vector.end(); it++)
	{
		std::string	temp = *it;
		if (temp.size() == 0)
			break ;
		std::cout << *it++ << std::endl;
	}
}
