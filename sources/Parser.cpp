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

	//CHECK for AUTHENTICATIOn when using ALL but PASS
	if (client.getState() < AUTHENTICATED && !(command[i].compare("PASS") == 0))
	{
		sendMessageToClient(client.getSocketFD(), "Authentication required! Please enter the server password with command /PASS.\n");
		return ;
	}

	if (command[i].compare("PASS") == 0)
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
		kick(client, command, i + 1);
	else if (command[i].compare("INVITE") == 0)
		invite(client, command, i + 1);
	else if (command[i].compare("TOPIC") == 0)
		topic(client, command, i + 1);
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
	if (command.size() < 2)
		return (sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_NEEDMOREPARAMS, client, "PASS")), 1);
	std::cout << client.getState() << std::endl;
	//DOES NOT CHANGE
	if (client.getState() >= AUTHENTICATED)
		return (sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_ALREADYREGISTERED, client)), 1);
		
	if (command[cmdNumber].compare(_password) == 0)
	{
		//MOVE THIS AFTER REGISTERED
		sendMessageToClient(client.getSocketFD(), createReplyToClient(RPL_WELCOME, client));
		
		client.setState(AUTHENTICATED);
	}
	else
		sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_PASSWDMISMATCH, client));
	return (0);
}

int	Server::kick(Client client, std::vector<std::string> command, size_t cmdNumber) //[:operatorName] KICK <channel> <user> [:<comment>]
{
	std::vector<std::string>	users;
	std::string					channelName, comment = "for NO Reason";
	Channel						*toKickFrom;
	Client						*kicked;

	//CHECK AUTHORITY
	//	"<channel> :You're not channel operator" --> ERR_CHANOPRIVSNEEDED

	//CHECK NUMBER OF NECESSARY PARAMETERS
	if (command.size() < 3)
		return (sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_NEEDMOREPARAMS, client, "KICK")), 1);

	//CHECK THE NAME FORMAT FOR CHANNELS
	channelName = command[cmdNumber++];
	if (channelName[0] != '#')
		return (sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_BADCHANMASK, client, channelName)), 1);
	//CHECK IF CHANNEL EXISTS
	toKickFrom = getChannel(channelName);
	if (toKickFrom == NULL)
		return (sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_NOSUCHCHANNEL, client, channelName)), 1);

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
		//CHECK IF USER IST ON THAT CHANNEL
		kicked = toKickFrom->getUser(users[i]);
		if (kicked == NULL)
		{
			sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_USERNOTINCHANNEL, client, users[i], channelName));
			continue ;
		}
			

		//TO DO::::
		//remove from list
		
	}
	return (0);
}

int	Server::invite(Client client, std::vector<std::string> command, size_t cmdNumber) //[:operatorName] INVITE <nickname> <channel>
{
	std::string	nickname, channelName;
	Channel		*toInviteTo;
	Client		*invited = NULL;

	//CHECK CLIENT AUTHORITY
	//	"<channel> :You're not channel operator" --> ERR_CHANOPRIVSNEEDED (482)	

	

	//CHECK NUMBER OF NECESSARY PARAMETERS
	if (command.size() < 3)
		return (sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_NEEDMOREPARAMS, client, "INVITE")), 1);

	nickname = command[cmdNumber++];
	//CHECK IF CHANNEL EXISTS
	channelName = command[cmdNumber++];
	toInviteTo = getChannel(channelName);
	if (toInviteTo == NULL)
		return (sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_NOSUCHCHANNEL, client, channelName)), 1);

	//CHECK IF COMMAND ISSUER PART OF THAT CHANNEL
	if (toInviteTo->getUser(client.getNickname()) == NULL);
		return (sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_NOTONCHANNEL, client, channelName)), 1);

	//CHECK IF CLIENT IS PART OF THAT CHANNEL
	if (toInviteTo->getUser(nickname) != NULL)
		return (sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_USERONCHANNEL, client, nickname, channelName)), 1);

	if (cmdNumber < command.size())
		return (std::cerr << "TOO MANY PARAMETERS" << std::endl, 1);

	//CHECK CHANNEL MODE (INVITE ONLY)	// ERR_CHANOPRIVSNEEDED (482)

	invited = getClient(nickname);
	if (invited == NULL)
		return (sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_NOSUCHNICK, client, nickname)), 1);


	// RPL_INVITING (341)
	// When the invite is successful, the server MUST send a RPL_INVITING numeric to the command issuer,
	// and an INVITE message, with the issuer as <source>?????????, to the target user. Other channel members SHOULD NOT be notified.

	sendMessageToClient(client.getSocketFD(), createReplyToClient(RPL_INVITING, client, nickname, channelName));
	//invited needs to get reassigend by pais function Server::getUser(nickname);
	sendMessageToClient(invited->getSocketFD(), createReplyToClient(RPL_TOPIC, *invited, channelName, toInviteTo->getTopic()));

	//WAIT FOR JOIN FUNCTION
	//toInviteTo->addUser();

	// if (operatorName.size() != 0)
	// 	std::cout << operatorName << " invited the user " << nickname << " has been invited to Channel '" << channelName << "'." << std::endl;
	// else
	// 	std::cout << "The user " << nickname << " has been invited to Channel '" << channelName << "'." << std::endl;

	return (0);
}

int		Server::topic(Client client, std::vector<std::string> command, size_t cmdNumber) // TOPIC <channel> [<topic>]
{
	std::string	channelName, topic;
	Channel		*toTakeTopicFrom;

	//CHECK CLIENT AUTHORITY
	//	"<channel> :You're not channel operator" --> ERR_CHANOPRIVSNEEDED (482)

	//CHECK CLIENT AVAILABILITY
	//	" "<client> <channel> :You're not on that channel"" --> ERR_NOTONCHANNEL (442)

	//CHECK NUMBER OF NECESSARY PARAMETERS
	if (command.size() < 2)
		return (sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_NEEDMOREPARAMS, client, "INVITE")), 1);
	
	//CHECK IF CHANNEL EXISTS
	channelName = command[cmdNumber++];
	toTakeTopicFrom = getChannel(channelName);
	if (toTakeTopicFrom == NULL)
		return (sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_NOSUCHCHANNEL, client, channelName)), 1);

	//CHECK THE TOPIC
	if (command.size() == 2)
	{
		if (toTakeTopicFrom->getTopic().empty() == true)
			sendMessageToClient(client.getSocketFD(), createReplyToClient(RPL_NOTOPIC, client, channelName));
		else
			sendMessageToClient(client.getSocketFD(), createReplyToClient(RPL_TOPIC, client, channelName, toTakeTopicFrom->getTopic()));
	}
	else if (command.size() == 3)
	{
		if (command[cmdNumber][0] == ':' && command[cmdNumber].size() == 1)
		{
			toTakeTopicFrom->setTopic("");
			for (size_t i = 0; i < toTakeTopicFrom->getChannelUsers().size(); i++)
				sendMessageToClient(client.getSocketFD(), createReplyToClient(RPL_NOTOPIC, client, channelName));
		}
		else
		{
			toTakeTopicFrom->setTopic(command[cmdNumber].substr(1, command[cmdNumber].length() -1));
			for (size_t i = 0; i < toTakeTopicFrom->getChannelUsers().size(); i++)
				sendMessageToClient(client.getSocketFD(), createReplyToClient(RPL_TOPIC, client, channelName, toTakeTopicFrom->getTopic()));
		}
	}
	else
		return (std::cerr << "TOO MANY PARAMETERS" << std::endl, -1);
	return (0);
}

// int		Server::mode( size_t cmdNumber)


std::string Server::createReplyToClient(int messageCode, Client client)
{
	std::string	returnMessage = "";

	if (messageCode == RPL_WELCOME)
	{
		returnMessage += client.getUsername();
		returnMessage += " :Welcome to the <networkname> Network, "; 
		returnMessage += client.getNickname();
		returnMessage += "[";
		returnMessage += client.getUsername();
		returnMessage += "@";
		returnMessage += client.getHostname();
		returnMessage += "]";
		returnMessage += "\r\n";
	}
	else if (messageCode == ERR_ALREADYREGISTERED)
	{
		returnMessage += client.getUsername();
		returnMessage += " :You may not reregister"; 
		returnMessage += "\r\n";
	}
	else if (messageCode == ERR_PASSWDMISMATCH)
	{
		returnMessage += client.getUsername();
		returnMessage += " :Password incorrect"; 
		returnMessage += "\r\n";
	}
	return (returnMessage);
}


std::string Server::createReplyToClient(int messageCode, Client client, std::string argument)
{
	std::string	returnMessage = "";

	if (messageCode == RPL_NOTOPIC)
	{
		returnMessage += client.getUsername();
		returnMessage += " ";
		returnMessage += argument;
		returnMessage += " :No topic is set\r\n";
	}
	else if (messageCode == ERR_NOSUCHNICK)
	{
		returnMessage += client.getUsername();
		returnMessage += " ";
		returnMessage += argument;
		returnMessage += " :No such nick/channel\r\n";
	}
	else if (messageCode == ERR_NOSUCHCHANNEL)
	{
		returnMessage += client.getUsername();
		returnMessage += " ";
		returnMessage += argument;
		returnMessage += " :No such channel\r\n";
	}
	else if (messageCode == ERR_NEEDMOREPARAMS)
	{
		returnMessage += client.getUsername();
		returnMessage += " ";
		returnMessage += argument;
		returnMessage += " :Not enough parameters\r\n";
	}
	else if (messageCode == ERR_NOTONCHANNEL)
	{
		returnMessage += client.getUsername();
		returnMessage += " ";
		returnMessage += argument;
		returnMessage += " :You're not on that channel\r\n";
	}
	else if (messageCode == ERR_BADCHANMASK)
	{
		returnMessage += client.getUsername();
		returnMessage += " ";
		returnMessage += argument;
		returnMessage += " :Bad Channel Mask\r\n";
	}
	else if (messageCode == ERR_CHANOPRIVSNEEDED)
	{
		returnMessage += client.getUsername();
		returnMessage += " ";
		returnMessage += argument;
		returnMessage += " :You're not channel operator\r\n";
	}
	return (returnMessage);
}

std::string Server::createReplyToClient(int messageCode, Client client, std::string arg1, std::string arg2)
{
	std::string	returnMessage = "";

	if (messageCode == RPL_TOPIC)
	{
		returnMessage += client.getUsername();
		returnMessage += " ";
		returnMessage += arg1;
		returnMessage += " :";
		returnMessage += arg2;
		returnMessage += "\r\n";
	}
	else if (messageCode == RPL_INVITING)
	{
		returnMessage += client.getUsername();
		returnMessage += " ";
		returnMessage += arg1;
		returnMessage += " ";
		returnMessage += arg2;
		returnMessage += "\r\n";
	}
	else if (messageCode == ERR_USERNOTINCHANNEL)
	{
		returnMessage += client.getUsername();
		returnMessage += " ";
		returnMessage += arg1;
		returnMessage += " ";
		returnMessage += arg2;
		returnMessage += " :They aren't on that channel\r\n";
	}
	else if (messageCode == ERR_USERONCHANNEL)
	{
		returnMessage += client.getUsername();
		returnMessage += " ";
		returnMessage += arg1;
		returnMessage += " ";
		returnMessage += arg2;
		returnMessage += " :is already on channel\r\n";
	}
	return (returnMessage);
}

