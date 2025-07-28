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

#include "../headers/Server.hpp"

void	Server::handleInput(Client client, std::string input)
{
	std::vector<std::string>	command;

	parseStringToVector(input, &command, " \f\n\r\t\v");
	// printVector(&command);
	executeCommand(client, command);
}

void    Server::parseStringToVector(std::string &input, std::vector<std::string> *vector, const char *delimiters)
{
	for (char* token = std::strtok((char *)input.c_str(), delimiters); token; token = std::strtok(NULL, delimiters))
	{
		if (token[0] == '\0')
			vector->push_back("");	
		else
			vector->push_back(token);
	}
		
}

void	Server::executeCommand(Client client, std::vector<std::string> command)
{
	std::string	operatorName = "";
	size_t 		i = 0;

	//DOES OPERATOR_NAME EXIST?
	if (command[i][0] == ':')
	{
		operatorName = command[0];
		i = 1;
	}

	if (command[i].compare("PASS") != 0 && client.getState() < AUTHENTICATED)
	{
		sendMessageToClient(client.getSocketFD(), "Authentication required! Please enter the server password with command /PASS.");
		return ;
	}

	//CHECK for AUTHENTICATIOn when using ALL but PASS

	if (command.size() == 0)
		return ;
	else if (command[i].compare("PASS") == 0)
		pass(client, command, i + 1);
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
		topic(command, i + 1);
	else if (command[i].compare("MODE") == 0)
		std::cout << "MODE" << std::endl; // -i -t -k -o -l
	else
		std::cerr << "Command NOT found" << std::endl;
}

void Server::printVector(std::vector<std::string> vector)
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


int		Server::pass(Client client, std::vector<std::string> command, size_t cmdNumber)
{
	if (command[cmdNumber].compare(_password) == 0)
	{
		std::string welcomeMessage = ":localhost 001 "; // 001 is the RPL_WELCOME code
		welcomeMessage += client.getNickname();
		welcomeMessage += " :Welcome to the StePiaAn Network, ";
		welcomeMessage += client.getFullIdentifier();
		welcomeMessage += "\r\n";
		sendMessageToClient(client.getSocketFD(), welcomeMessage.c_str()); // Welcome message
	}
	else
	{
		sendMessageToClient(client.getSocketFD(), "Incorrect password. Please try again.\r\n"); // send error message to IRC client
	}
	return (0);
}

int	Server::kick(std::vector<std::string> command, size_t cmdNumber, std::string operatorName) //[:operatorName] KICK <channel> <user> [:<comment>]
{
	std::vector<std::string>	users;
	std::string					channel, comment = "for NO Reason";
	Channel						*toKickFrom;

	//CHECK AUTHORITY
	//	"<channel> :You're not channel operator" --> ERR_CHANOPRIVSNEEDED

	if (command.size() < 3)
		return (std::cerr << "KICK: MISSING PARAMETERS" << std::endl, 461); //ERR_NEEDMOREPARAMS == 461
	
	//CHECK IF CHANNEL EXISTS IF NOT SKIP OR END COMD
	channel = command[cmdNumber++];
	for (size_t it = 0; it < _channels.size(); it++)
	{
		if (_channels[it]->getName().compare(channel) == 0)
		{
			toKickFrom = _channels[it];
			break ;
		}
		if (it == _channels.size() - 1)
			return (std::cerr << "<client> <channel> :No such channel" << std::endl, 402); //ERR_NOSUCHCHANNEL == 402
	}

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
		//CHECK IF USER EXISTS IF NOT SKIP
		std::vector<std::string> nameList = toKickFrom->getChannelUsers();
		for (size_t it = 0; it < nameList.size(); it++)
		{
			if (nameList[it].compare(users[i]) == 0)
				break ;
			if (it == nameList.size() - 1)
				return (std::cerr << "<client> <nick> <channel> :They aren't on that channel" << std::endl, 441); //ERR_USERNOTINCHANNEL (441)
		}


		if (operatorName.size() != 0)
			std::cout << "KICKED the user '" << users[i] << "' out of channel '" << channel << "' by operator " << operatorName << " because of " << comment << "!" << std::endl;
		else
			std::cout << "KICKED the user " << users[i] << " out of channel '" << channel << "' because of " << comment << "!" << std::endl;
	}

    //	ERR_BADCHANMASK                 

	return (0);

}

int	Server::invite(std::vector<std::string> command, size_t cmdNumber, std::string operatorName) //[:operatorName] INVITE <nickname> <channel>
{
	std::string	nickname, channel;

	//CHECK CLIENT AUTHORITY
	//	"<channel> :You're not channel operator" --> ERR_CHANOPRIVSNEEDED (482)

	//CHECK CLIENT AVAILABILITY
	//	" "<client> <channel> :You're not on that channel"" --> ERR_NOTONCHANNEL (442)

	if (command.size() < 3)
		return (std::cerr << "INVITE: MISSING PARAMETERS" << std::endl, 461); // ERR_NEEDMOREPARAMS

	nickname = command[cmdNumber++];

	//CHECK IF CHANNEL EXISTS == // ERR_NOSUCHCHANNEL (403)
	channel = command[cmdNumber++];

	if (cmdNumber < command.size())
		return (std::cerr << "TOO MANY PARAMETERS" << std::endl, 1);


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

int		Server::topic(std::vector<std::string> command, size_t cmdNumber) // TOPIC <channel> [<topic>]
{
	std::string	channel, topic;
	Channel		*toTakeTopicFrom;

	//CHECK CLIENT AUTHORITY
	//	"<channel> :You're not channel operator" --> ERR_CHANOPRIVSNEEDED (482)

	//CHECK CLIENT AVAILABILITY
	//	" "<client> <channel> :You're not on that channel"" --> ERR_NOTONCHANNEL (442)

	if (command.size() < 2)
		return (std::cerr << "TOPIC: MISSING PARAMETERS" << std::endl, 461); //ERR_NEEDMOREPARAMS == 461
	
	//CHECK IF CHANNEL EXISTS IF NOT SKIP OR END COMD
	channel = command[cmdNumber++];
	for (size_t it = 0; it < _channels.size(); it++)
	{
		if (_channels[it]->getName().compare(channel) == 0)
		{
			toTakeTopicFrom = _channels[it];
			break ;
		}
		if (it == _channels.size() - 1)
			return (std::cerr << "<client> <channel> :No such channel" << std::endl, 403); //ERR_NOSUCHCHANNEL == 403
	}

	//CHECK Status of topic
	if (command.size() == 2)
		std::cout << "Current topic is: '" << topic << "'!" << std::endl; // --> RPL_TOPIC (332) && RPL_TOPICWHOTIME (333)
	else if (command.size() == 3)
	{
		if (command[2][0] == ':' && command[2].size() == 1)
		{
			toTakeTopicFrom->setTopic(""); // --> RPL_NOTOPIC (331)
		}
		else
		{
			toTakeTopicFrom->setTopic(command[2].substr(1, command[2].length() -1));
			//SEND TOPIC COMMAND TO EVErY CLIENT iN THE CHANNEL // --> RPL_TOPIC (332) && RPL_TOPICWHOTIME (333)
		}
	}
	else
		return (std::cerr << "TOO MANY PARAMETERS" << std::endl, -1);
	return (0);
}

// int		Server::mode( size_t cmdNumber)
