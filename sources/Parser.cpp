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
		parseStringToVector(input, &command, " \f\n\r\t\v");
		// printVector(&command);
		executeCommand(command);
		command.clear();
	}
}

void    parseStringToVector(std::string &input, std::vector<std::string> *vector, const char *delimiters)
{
	for (char* token = std::strtok((char *)input.c_str(), delimiters); token; token = std::strtok(NULL, delimiters))
	{
		if (token[0] == '\0')
			vector->push_back("");	
		else
			vector->push_back(token);
	}
		
}

void	executeCommand(std::vector<std::string> command)
{
	std::string	operatorName = "";
	size_t 		i = 0;

	//DOES OPERATOR_NAME EXIST?
	if (command[i][0] == ':')
	{
		operatorName = command[0];
		i = 1;
	}

	if (command.size() == 0)
		return ;
	else if (command[i].compare("AUTHENTICATE") == 0)
		std::cout << "AUTHENTICATE" << std::endl;
	else if (command[i].compare("NICK") == 0)
		std::cout << "NICK" << std::endl;
	else if (command[i].compare("USER") == 0)
		std::cout << "USER" << std::endl;
	else if (command[i].compare("JOIN") == 0)
		std::cout << "JOIN" << std::endl;
	else if (command[i].compare("PRIVMSG") == 0)
		std::cout << "PRIVMSG" << std::endl; // (is the same for recieving?)
	else if (command[i].compare("KICK") == 0)
		kick(command, i + 1, operatorName);
	else if (command[i].compare("INVITE") == 0)
		invite(command, i + 1, operatorName);
	else if (command[i].compare("TOPIC") == 0)
		std::cout << "TOPIC" << std::endl;
	else if (command[i].compare("MODE") == 0)
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

size_t	kick(std::vector<std::string> command, size_t cmdNumber, std::string operatorName) //:operatorName <channel> <user> [:<comment>]
{
	std::vector<std::string>	users;
	std::string					channel, comment = "for NO Reason";

	//CHECK AUTHORITY
	//	"<channel> :You're not channel operator" --> ERR_CHANOPRIVSNEEDED

	if (command.size() < 3)
		return (std::cerr << "KICK: MISSING PARAMETERS" << std::endl, 461); //ERR_NEEDMOREPARAMS == 461
	
	//CHECK IF CHANNEL EXISTS IF NOT SKIP OR END COMD --> ERR_NOSUCHCHANNEL
	channel = command[cmdNumber++];

	//Parsing the command argument into user name(s) stored in a vector
	if (command[cmdNumber].find(","))
		parseStringToVector(command[cmdNumber++], &users, ",");
	else
		users.push_back(command[cmdNumber++]);

	//Check for a KICK message (that starts with ':')
	if (cmdNumber < command.size())
	{
		if (command[cmdNumber][0] == ':')
			comment = command[cmdNumber];
		cmdNumber++;
	}

	if (cmdNumber < command.size())
		return (std::cerr << "TOO MANY PARAMETERS" << std::endl, 1);
	
	//Looping through the channels and users to be KICKED
	for (size_t i = 0; i < users.size(); i++)
	{
		//CHECK IF USER EXISTS IF NOT SKIP OR END COMD --> ERR_NOTONCHANNEL
		if (operatorName.size() != 0)
			std::cout << "KICKED the user '" << users[i] << "' out of channel '" << channel << "' by operator " << operatorName << " because of " << comment << "!" << std::endl;
		else
			std::cout << "KICKED the user " << users[i] << " out of channel '" << channel << "' because of " << comment << "!" << std::endl;
	}

    //	ERR_BADCHANMASK                 

	return (0);

}

size_t	invite(std::vector<std::string> command, size_t cmdNumber, std::string operatorName) //:operatorName <nickname> <channel>
{
	std::string	nickname, channel;

	if (command.size() < 3)
		return (std::cerr << "INVITE: MISSING PARAMETERS" << std::endl, 461); // ERR_NEEDMOREPARAMS

	nickname = command[cmdNumber++];

	//CHECK IF CHANNEL EXISTS == // ERR_NOSUCHCHANNEL (403)
	channel = command[cmdNumber++];

	if (cmdNumber < command.size())
		return (std::cerr << "TOO MANY PARAMETERS" << std::endl, 1);

	//CHECK IF USER IS ON THAT CHANNEL == // ERR_NOTONCHANNEL (442)

	//CHECK AUTHORITY
	//	"<channel> :You're not channel operator" // ERR_CHANOPRIVSNEEDED (482)

	//CHECK CHANNEL MODE (INVITE ONLY)	// ERR_CHANOPRIVSNEEDED (482)

	//CHECK IF USER IS ALREADY ON THAT CHANNEL // ERR_USERONCHANNEL (443)

	// RPL_INVITING (341)
	// When the invite is successful, the server MUST send a RPL_INVITING numeric to the command issuer,
	// and an INVITE message, with the issuer as <source>, to the target user. Other channel members SHOULD NOT be notified.

	if (operatorName.size() != 0)
		std::cout << operatorName << " invited the user " << nickname << " has been invited to Channel '" << channel << "'." << std::endl;
	else
		std::cout << "The user " << nickname << " has been invited to Channel '" << channel << "'." << std::endl;

	return (0);
}
