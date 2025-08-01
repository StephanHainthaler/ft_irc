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

void	Server::handleInput(Client *client, std::string input)
{
	std::vector<std::string>	command;

	parseStringToVector(input, &command, " \f\n\r\t\v");
	executeCommand(client, command);
}

void    Server::parseStringToVector(std::string &input, std::vector<std::string> *vector, const char *delimiters)
{
	// char *copy = NULL;
	// input.copy(copy, input.length(), 0);

	for (char* token = std::strtok((char *)input.c_str(), delimiters); token; token = std::strtok(NULL, delimiters))
	{
		// if (token[0] == ':')
		// {
		// 	std::cout << "Found a : in " << token << std::endl;
		// 	std::cout << "Hackstack: " << copy << ", Needle: " << token << std::endl;
		// 	std::cout << std::strstr((char *)copy, token) << std::endl;
		// 	token = std::strstr((char *)copy, token);
		// 	vector->push_back(token);
		// 	return ;
		// }
		vector->push_back(token);
	}
		
}

void	Server::executeCommand(Client *client, std::vector<std::string> command)
{
	if (client->getState() < AUTHENTICATED && !(command[0].compare("PASS") == 0))
	{
		sendMessageToClient(client->getSocketFD(), "Authentication required! Please enter the server password with command PASS.");
		return ;
	}
	else if (command[0].compare("PASS") == 0)
		pass(*client, command, 1);
	else if (command[0].compare("NICK") == 0)
		nick(*client, command, 1);
	else if (command[0].compare("USER") == 0)
		user(*client, command, 1);
	else if (command[0].compare("JOIN") == 0)
		join(*client, command, 1);
	else if (command[0].compare("PRIVMSG") == 0)
		std::cout << "PRIVMSG" << std::endl;
	else if (command[0].compare("KICK") == 0)
		kick(*client, command, 1);
	else if (command[0].compare("INVITE") == 0)
		invite(*client, command, 1);
	else if (command[0].compare("TOPIC") == 0)
		topic(*client, command, 1);
	else if (command[0].compare("MODE") == 0)
		std::cout << "MODE" << std::endl;
	else
		sendMessageToClient(client->getSocketFD(), createReplyToClient(ERR_UNKNOWNCOMMAND, *client, command[0]));
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

int		Server::pass(Client &client, std::vector<std::string> command, size_t cmdNumber)
{
	if (command.size() < 2)
		return (sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_NEEDMOREPARAMS, client, "PASS")), 1);
	else if (client.getState() >= AUTHENTICATED)
		return (sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_ALREADYREGISTERED, client)), 1);
	else if (command[cmdNumber].compare(_password) == 0)
	{
		client.setState(AUTHENTICATED);
		sendMessageToClient(client.getSocketFD(), "You are now authenticated! Please input your nickname and username with the NICK and USER command respectively!");
	}
	else
		sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_PASSWDMISMATCH, client));
	return (0);
}

int	Server::nick(Client &client, std::vector<std::string> command, size_t cmdNumber)
{
	if (client.isNickValid(command[cmdNumber]) != 0)
		return (1);
	client.setNick(command[cmdNumber]);
	sendMessageToClient(client.getSocketFD(), "You are now known as " + client.getNickname());
	if (client.getNickname().empty() == false && client.getUsername().empty() == false && client.getRealname().empty() == false)
	{
		client.setState(REGISTERED);
		sendMessageToClient(client.getSocketFD(), createReplyToClient(RPL_WELCOME, client));
	}
	return (0);
}

int	Server::user(Client &client, std::vector<std::string> command, size_t cmdNumber)
{
	if (!(client.getState() < REGISTERED))
		return (1);

	if (command.size() < 5)
		return (sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_NEEDMOREPARAMS, client, "USER")), 1);
	
	//  DO CHECKS FOR 0 an *
	client.setUser(command[cmdNumber], 0, '*', command[cmdNumber + 3]);
	if (client.getNickname().empty() == false && client.getUsername().empty() == false && client.getRealname().empty() == false)
	{
		client.setState(REGISTERED);
		sendMessageToClient(client.getSocketFD(), createReplyToClient(RPL_WELCOME, client));
	}
	return (0);
}

int	Server::join(Client &client, std::vector<std::string> command, size_t cmdNumber) //JOIN <channel>{,<channel>} [<key>{,<key>}]
{
	std::vector<std::string>	channelNames, keyNames;
	Channel						*toJoinTo;

	if (command.size() < 2)
		return (sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_NEEDMOREPARAMS, client, "JOIN")), 1);
	
	//Parsing the command argument into channel name(s) stored in a vector
	if (command[cmdNumber].find(","))
		parseStringToVector(command[cmdNumber++], &channelNames, ",");
	if (channelNames.size() == 0)
		return (sendMessageToClient(client.getSocketFD(), ("No Channels given!")), 1);

	//Parsing the command argument into user name(s) stored in a vector
	if (cmdNumber < command.size())
		if (command[cmdNumber].find(","))
			parseStringToVector(command[cmdNumber++], &keyNames, ",");

	if (cmdNumber < command.size()) 
		return (std::cerr << "TOO MANY PARAMETERS" << std::endl, 1);

	//CHECK IN HOW MANY CHANNELS THE USER IS CURRENTLY IN -->TOOMANYCHANNELS

	for (size_t i = 0; i < channelNames.size(); i++)
	{
		// if (channelNames.size() == 1 && channelNames[i].compare("0") == 0)
		// {
		// 	//PART WIH ALL CHANNELS
		// }
		if (channelNames[i][0] != '#')
		{
			sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_BADCHANMASK, client, channelNames[i]));
			continue ;
		}
		toJoinTo = getChannel(channelNames[i]);
		// if (toJoinTo == NULL)
		// {
		// 	sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_NOSUCHCHANNEL, client, channelName));
		// 	continue ;
		// }
		if (toJoinTo == NULL)
		{
			Channel *newChannel = new Channel(channelNames[i], "", "");
			addChannel(newChannel);
			newChannel->addUser(&client);
			newChannel->addOperator(&client);
			client.joinChannel(newChannel->getName());
			// std::cout << ":" << client.getFullIdentifier() << " JOIN :" << newChannel->getName() << std::endl;
			sendMessageToClient(client.getSocketFD(), ":" + client.getFullIdentifier() + " JOIN :" + newChannel->getName());
		}
		else
		{
			if (toJoinTo->getUser(client.getNickname()) != NULL)
			{
				sendMessageToClient(client.getSocketFD(), "You are already on that Channel");
				continue ;
			}
			else if (toJoinTo->hasMode('k') == true)
			{
				if (keyNames.size() == 0 || i != keyNames.size() - 1 || keyNames[i].compare(toJoinTo->getChannelKey()) != 0)
				{
					sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_BADCHANNELKEY, client, channelNames[i]));
					continue ;
				}
			}
			else if (toJoinTo->hasMode('b') == true)
			{
				//CHECK IF BANNED ON CHANNELLSIT
				// if (????)
				// {
				// 	sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_BANNEDFROMCHAN, client, channelNames[i]));
				// 	continue ;
				// }
			}
			else if (toJoinTo->hasMode('l') == true)
			{
				if (toJoinTo->getChannelUsers().size() >= toJoinTo->getUserLimit())
				{
					sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_BANNEDFROMCHAN, client, channelNames[i]));
					continue ;
				}
			}
			else if (toJoinTo->hasMode('i') == true)
			{
				sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_INVITEONLYCHAN, client, channelNames[i]));
				continue ;
			}
			toJoinTo->addUser(&client);
			//	1. actually to all in the channel
			sendMessageToClient(client.getSocketFD(), client.getFullIdentifier() + " JOIN " + toJoinTo->getName());

			//	2. Send topic of channel
			if (toJoinTo->getTopic().empty() == true)
				sendMessageToClient(client.getSocketFD(), createReplyToClient(RPL_NOTOPIC, client, toJoinTo->getName()));
			else
				sendMessageToClient(client.getSocketFD(), createReplyToClient(RPL_TOPIC, client, toJoinTo->getName(), toJoinTo->getTopic()));

			// 3. 
				sendMessageToClient(client.getSocketFD(), createReplyToClient(RPL_NAMREPLY, client, toJoinTo->getName(), toJoinTo->getNamesOfChannelMembers()));
				sendMessageToClient(client.getSocketFD(), createReplyToClient(RPL_ENDOFNAMES, client, toJoinTo->getName()));

		}
	}
	// for (size_t i = 0; i < _channels.size(); i++)
	// {
	// 	std::cout << "Channel " << i + 1 << ": " << _channels[i]->getName() << std::endl;
	// }
	return (0);
}

int	Server::kick(Client &client, std::vector<std::string> command, size_t cmdNumber) //KICK <channel> <user>[,<user>,...] [:<comment>]
{
	std::vector<std::string>	users;
	std::string					channelName, comment = "for NO Reason";
	Channel						*toKickFrom;
	Client						*toBeKicked;

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
	if (users.size() == 0)
		return (sendMessageToClient(client.getSocketFD(), ("No Channels given!")), 1);

	//Check for a KICK message (that starts with ':')
	if (cmdNumber < command.size())
	{
		if (command[cmdNumber][0] == ':')
			comment = command[cmdNumber];
		cmdNumber++;
	}
	if (cmdNumber < command.size())
		return (std::cerr << "TOO MANY PARAMETERS" << std::endl, 1);
	
	//Looping through the channels and users to be toBeKicked
	for (size_t i = 0; i < users.size(); i++)
	{
		//CHECK IF USER IST ON THAT CHANNEL
		toBeKicked = toKickFrom->getUser(users[i]);
		if (toBeKicked == NULL)
		{
			sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_USERNOTINCHANNEL, client, users[i], channelName));
			continue ;
		}
			

		//TO DO::::
		//remove from list
		
	}
	return (0);
}

int	Server::invite(Client &client, std::vector<std::string> command, size_t cmdNumber) //INVITE <nickname> <channel>
{
	std::string	nickname, channelName;
	Channel		*toInviteTo;
	Client		*toBeInvited = NULL;

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
	if (toInviteTo->getUser(client.getNickname()) == NULL)
		return (sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_NOTONCHANNEL, client, channelName)), 1);

	//CHECK IF CLIENT IS PART OF THAT CHANNEL
	if (toInviteTo->getUser(nickname) != NULL)
		return (sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_USERONCHANNEL, client, nickname, channelName)), 1);

	if (cmdNumber < command.size())
		return (std::cerr << "TOO MANY PARAMETERS" << std::endl, 1);

	//CHECK CHANNEL MODE (INVITE ONLY)	// ERR_CHANOPRIVSNEEDED (482)

	toBeInvited = getClient(nickname);
	if (toBeInvited == NULL)
		return (sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_NOSUCHNICK, client, nickname)), 1);


	// RPL_INVITING (341)
	// When the invite is successful, the server MUST send a RPL_INVITING numeric to the command issuer,
	// and an INVITE message, with the issuer as <source>?????????, to the target user. Other channel members SHOULD NOT be notified.

	sendMessageToClient(client.getSocketFD(), createReplyToClient(RPL_INVITING, client, nickname, channelName));
	//toBeInvited needs to get reassigend by pais function Server::getUser(nickname);
	sendMessageToClient(toBeInvited->getSocketFD(), createReplyToClient(RPL_TOPIC, *toBeInvited, channelName, toInviteTo->getTopic()));

	//WAIT FOR JOIN FUNCTION
	//toInviteTo->addUser();

	// if (operatorName.size() != 0)
	// 	std::cout << operatorName << " invited the user " << nickname << " has been invited to Channel '" << channelName << "'." << std::endl;
	// else
	// 	std::cout << "The user " << nickname << " has been invited to Channel '" << channelName << "'." << std::endl;

	return (0);
}

int	Server::topic(Client &client, std::vector<std::string> command, size_t cmdNumber) //TOPIC <channel> [<topic>]
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


std::string Server::createReplyToClient(int messageCode, Client &client)
{
	std::string	returnMessage = "";

	if (messageCode == RPL_WELCOME)
		returnMessage = client.getNickname() + " :Welcome to the Internet Relay Chat Network, " + client.getFullIdentifier(); 
	else if (messageCode == ERR_ALREADYREGISTERED)
		returnMessage = client.getNickname() + " :You may not reregister";
	else if (messageCode == ERR_PASSWDMISMATCH)
	{
		if (client.getState() < REGISTERED)
			returnMessage = "Password incorrect. Please try again.";
		else
			returnMessage = client.getNickname() + " :Password incorrect"; //SHPUlD THEpreTicAlly Never APpeaR
	}
	return (returnMessage);
}

std::string Server::createReplyToClient(int messageCode, Client &client, std::string argument)
{
	std::string	returnMessage = "";

	if (messageCode == RPL_NOTOPIC)
		returnMessage = client.getNickname() + " " + argument + " :No topic is set";
	else if (messageCode == RPL_ENDOFNAMES)
		returnMessage = client.getNickname() + " " + argument + " :End of /NAMES list";
	else if (messageCode == ERR_NOSUCHNICK)
		returnMessage = client.getNickname() + " " + argument + " :No such nick/channel";
	else if (messageCode == ERR_NOSUCHCHANNEL)
		returnMessage = client.getNickname() + " " + argument + " :No such channel";
	else if (messageCode == ERR_TOOMANYCHANNELS)
		returnMessage = client.getNickname() + " " + argument + " :You have joined too many channels";
	else if (messageCode == ERR_UNKNOWNCOMMAND)
		returnMessage = client.getNickname() + " " + argument + " :Unknown command";
	else if (messageCode == ERR_NOTONCHANNEL)
		returnMessage = client.getNickname() + " " + argument + " :You're not on that channel";
	else if (messageCode == ERR_NEEDMOREPARAMS)
		returnMessage = client.getNickname() + " " + argument + " :Not enough parameters";
	else if (messageCode == ERR_CHANNELISFULL)
		returnMessage = client.getNickname() + " " + argument + " :Cannot join channel (+l)";
	else if (messageCode == ERR_INVITEONLYCHAN)
		returnMessage = client.getNickname() + " " + argument + " :Cannot join channel (+i)";
	else if (messageCode == ERR_BANNEDFROMCHAN)
		returnMessage = client.getNickname() + " " + argument + " :Cannot join channel (+b)";
	else if (messageCode == ERR_BADCHANNELKEY)
		returnMessage = client.getNickname() + " " + argument + " :Cannot join channel (+k)";
	else if (messageCode == ERR_BADCHANMASK)
		returnMessage = client.getNickname() + " " + argument + " :Bad Channel Mask";
	else if (messageCode == ERR_CHANOPRIVSNEEDED)
		returnMessage = client.getNickname() + " " + argument + " :You're not channel operator";
	return (returnMessage);
}

std::string Server::createReplyToClient(int messageCode, Client &client, std::string arg1, std::string arg2)
{
	std::string	returnMessage = "";

	if (messageCode == RPL_TOPIC)
		returnMessage = client.getNickname() + " " + arg1 + " :" + arg2;
	else if (messageCode == RPL_INVITING)
		returnMessage = client.getNickname() + " " + arg1 + " " + arg2;
	else if (messageCode == RPL_NAMREPLY)
		returnMessage = client.getNickname() + " = " + arg1 + " :" + arg2;
	else if (messageCode == ERR_USERNOTINCHANNEL)
		returnMessage = client.getNickname() + " " + arg1 + " " + arg2 + " :They aren't on that channel";
	else if (messageCode == ERR_USERONCHANNEL)
		returnMessage = client.getNickname() + " " + arg1 + " " + arg2 + " :is already on channel";
	return (returnMessage);
}
