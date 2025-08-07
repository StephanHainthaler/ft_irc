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

void	Server::handleInput(Client &client, std::string input)
{
	std::vector<std::string>	command;

	parseStringToVector(input, &command, " \f\n\r\t\v");
	if (command.empty() == true)
		return ;
	else if (client.getState() < AUTHENTICATED && command[0] != "PASS")
		sendMessageToClient(client.getSocketFD(), "Authentication required! Please enter the server password with command PASS.");
	else if (client.getState() == AUTHENTICATED && command[0] != "PASS" && command[0] != "NICK" && command[0] != "USER")
		sendMessageToClient(client.getSocketFD(), ERR_NOTREGISTERED(getName(), client.getClientName()));
	else
		executeCommand(client, command);
	if (client.getState() < REGISTERED && client.getNickname() != "*" && client.getUsername() != "*")
	{
		client.setState(REGISTERED);
		sendMessageToClient(client.getSocketFD(), RPL_WELCOME(getName(), client.getNickname(), client.getNickname(), client.getUsername(), client.getHostname()));
	}
}

void    Server::parseStringToVector(std::string &input, std::vector<std::string> *vector, const char *delimiters)
{
	for (char* token = std::strtok((char *)input.c_str(), delimiters); token; token = std::strtok(NULL, delimiters))
		vector->push_back(token);
}

void	Server::executeCommand(Client &client, std::vector<std::string> command)
{
	if (command[0].compare("PASS") == 0)
		pass(client, command, 1);
	else if (command[0].compare("NICK") == 0)
		nick(client, command, 1);
	else if (command[0].compare("USER") == 0)
		user(client, command, 1);
	else if (command[0].compare("JOIN") == 0)
		join(client, command, 1);
	else if (command[0].compare("PRIVMSG") == 0)
		std::cout << "PRIVMSG" << std::endl;
	else if (command[0].compare("KICK") == 0)
		kick(client, command, 1);
	else if (command[0].compare("INVITE") == 0)
		invite(client, command, 1);
	else if (command[0].compare("TOPIC") == 0)
		topic(client, command, 1);
	else if (command[0].compare("MODE") == 0)
		mode(client, command, 1);
	else if (command[0].compare("TEST") == 0)
		testAllNumericReplies(client.getSocketFD(), client);
	else
		sendMessageToClient(client.getSocketFD(), ERR_UNKNOWNCOMMAND(getName(), client.getClientName(), command[0]));
}

int		Server::pass(Client &client, std::vector<std::string> command, size_t cmdNumber)
{
	if (command.size() < 2)
		return (sendMessageToClient(client.getSocketFD(), ERR_NEEDMOREPARAMS(getName(), client.getNickname(), "PASS")), 1);
	else if (client.getState() >= AUTHENTICATED)
		return (sendMessageToClient(client.getSocketFD(), ERR_ALREADYREGISTERED(getName(), client.getNickname())), 1);
	else if (command[cmdNumber].compare(_password) != 0)
		return (sendMessageToClient(client.getSocketFD(), ERR_PASSWDMISMATCH(getName(), client.getNickname())), 1);
	client.setState(AUTHENTICATED);
	return (sendMessageToClient(client.getSocketFD(), "Authentication complete! Please input your nickname & username with NICK/USER."), 0);
}

int	Server::nick(Client &client, std::vector<std::string> command, size_t cmdNumber)
{
	if (command.size() <= cmdNumber)
		return (sendMessageToClient(client.getSocketFD(), ERR_NONICKNAMEGIVEN(getName(), client.getClientName())), 1);
	else if (client.isNickValid(command[cmdNumber]) == false)
		return (sendMessageToClient(client.getSocketFD(), ERR_ERRONEUSNICKNAME(getName(), client.getClientName(), command[cmdNumber])), 1);
	else if (isNicknameAvailable(command[cmdNumber], &client) == false)
		return (sendMessageToClient(client.getSocketFD(), ERR_NICKNAMEINUSE(getName(), client.getClientName(), command[cmdNumber])), 1);
	sendMessageToClient(client.getSocketFD(), MSG_NICK(client.getNickname(), command[cmdNumber]));
	client.setNick(command[cmdNumber]);
	return (0);
}

int	Server::user(Client &client, std::vector<std::string> command, size_t cmdNumber)
{
	if (command.size() < 5)
		return (sendMessageToClient(client.getSocketFD(), ERR_NEEDMOREPARAMS(getName(), client.getClientName(), "USER")), 1);
	else if (client.getState() >= REGISTERED)
		return (sendMessageToClient(client.getSocketFD(), ERR_ALREADYREGISTERED(getName(), client.getClientName())), 1);

	//  DO CHECKS FOR 0 an *
	client.setUser(command[cmdNumber], 0, '*', command[cmdNumber + 3]);
	return (0);
}

int	Server::join(Client &client, std::vector<std::string> command, size_t cmdNumber) //JOIN <channel>{,<channel>} [<key>{,<key>}]
{
	std::vector<std::string>	channelNames, keyNames;
	Channel						*toJoinTo;

	if (command.size() < 2)
		return (sendMessageToClient(client.getSocketFD(), ERR_NEEDMOREPARAMS(getName(), client.getClientName(), "JOIN")), 1);
	
	//Parsing the command argument into channel name(s) stored in a vector
	if (command[cmdNumber].find(","))
		parseStringToVector(command[cmdNumber++], &channelNames, ",");
	if (channelNames.size() == 0)
		return (1);

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
			sendMessageToClient(client.getSocketFD(), ERR_BADCHANMASK(getName(), client.getClientName(), channelNames[i]));
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
			Channel *newChannel = new Channel(channelNames[i], client);
			addChannel(newChannel);
			newChannel->addUser(&client);
			std::string temp = client.getNickname();
			newChannel->setOperator(temp, true);

			sendMessageToClient(client.getSocketFD(), MSG_JOIN(client.getNickname(), channelNames[i]));
			sendMessageToClient(client.getSocketFD(), MSG_MODE(getName(), channelNames[i], "+o", client.getNickname()));
			sendMessageToClient(client.getSocketFD(), RPL_NOTOPIC(getName(), client.getNickname(), channelNames[i]));
			sendMessageToClient(client.getSocketFD(), RPL_NAMREPLY(getName(), client.getNickname(), "=", channelNames[i], "@" + client.getNickname()));
			sendMessageToClient(client.getSocketFD(), RPL_ENDOFNAMES(getName(), client.getNickname(), channelNames[i]));
		}
		else
		{
			if (toJoinTo->getUser(client.getNickname()) != NULL)
				continue ;
			else if (toJoinTo->hasMode('k') == true)
			{
				if (keyNames.size() == 0 || i != keyNames.size() - 1 || keyNames[i].compare(toJoinTo->getChannelKey()) != 0)
				{
					sendMessageToClient(client.getSocketFD(), ERR_BADCHANNELKEY(getName(), client.getNickname(), channelNames[i]));
					continue ;
				}
			}
			// else if (toJoinTo->hasMode('b') == true)
			// {
			// 	//CHECK IF BANNED ON CHANNELLSIT
			// 	if (????)
			// 	{
			// 		sendMessageToClient(client.getSocketFD(), createReplyToClient(ERR_BANNEDFROMCHAN, client, channelNames[i]));
			// 		continue ;
			// 	}
			// }
			else if (toJoinTo->hasMode('l') == true)
			{
				if (toJoinTo->getChannelUsers().size() >= toJoinTo->getUserLimit())
				{
					sendMessageToClient(client.getSocketFD(), ERR_CHANNELISFULL(getName(), client.getNickname(), channelNames[i]));
					continue ;
				}
			}
			else if (toJoinTo->hasMode('i') == true)
			{
				sendMessageToClient(client.getSocketFD(), ERR_INVITEONLYCHAN(getName(), client.getNickname(), channelNames[i]));
				continue ;
			}
			toJoinTo->addUser(&client);
			//	1. actually to all in the channel
			sendMessageToClient(client.getSocketFD(), MSG_JOIN(client.getNickname(), channelNames[i]));


			//	2. Send topic of channel
			if (toJoinTo->getTopic().empty() == true)
				sendMessageToClient(client.getSocketFD(), RPL_NOTOPIC(getName(), client.getNickname(), channelNames[i]));
			else
				sendMessageToClient(client.getSocketFD(), RPL_TOPIC(getName(), client.getNickname(), channelNames[i], toJoinTo->getTopic()));

			// 3. 
			sendMessageToClient(client.getSocketFD(), RPL_NAMREPLY(getName(), client.getNickname(), "=", channelNames[i], toJoinTo->getNamesOfChannelMembers()));
			sendMessageToClient(client.getSocketFD(), RPL_ENDOFNAMES(getName(), client.getNickname(), channelNames[i]));
			sendMessageToChannel(&client, toJoinTo, RPL_NAMREPLY(getName(), client.getNickname(), "=", channelNames[i], toJoinTo->getNamesOfChannelMembers()));
			sendMessageToChannel(&client, toJoinTo, RPL_ENDOFNAMES(getName(), client.getNickname(), channelNames[i]));
		}
	}
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
		return (sendMessageToClient(client.getSocketFD(), ERR_NEEDMOREPARAMS(getName(), client.getClientName(), "KICK")), 1);

	//CHECK THE NAME FORMAT FOR CHANNELS
	channelName = command[cmdNumber++];
	if (channelName[0] != '#')
		return (sendMessageToClient(client.getSocketFD(), ERR_BADCHANMASK(getName(), client.getClientName(), channelName)), 1);
	//CHECK IF CHANNEL EXISTS
	toKickFrom = getChannel(channelName);
	if (toKickFrom == NULL)
		return (sendMessageToClient(client.getSocketFD(), ERR_NOSUCHCHANNEL(getName(), client.getClientName(), channelName)), 1);

	//Parsing the command argument into user name(s) stored in a vector
	if (command[cmdNumber].find(","))
		parseStringToVector(command[cmdNumber++], &users, ",");
	if (users.size() == 0)
		return (1);

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
			sendMessageToClient(client.getSocketFD(), ERR_USERNOTINCHANNEL(getName(), client.getClientName(), users[i], channelName));
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
		return (sendMessageToClient(client.getSocketFD(), ERR_NEEDMOREPARAMS(getName(), client.getClientName(), "INVITE")), 1);

	nickname = command[cmdNumber++];
	//CHECK IF CHANNEL EXISTS
	channelName = command[cmdNumber++];
	toInviteTo = getChannel(channelName);
	if (toInviteTo == NULL)
		return (sendMessageToClient(client.getSocketFD(), ERR_NOSUCHCHANNEL(getName(), client.getClientName(), channelName)), 1);

	//CHECK IF COMMAND ISSUER PART OF THAT CHANNEL
	if (toInviteTo->getUser(client.getNickname()) == NULL)
		return (sendMessageToClient(client.getSocketFD(), ERR_NOTONCHANNEL(getName(), client.getClientName(), channelName)), 1);

	//CHECK IF CLIENT IS PART OF THAT CHANNEL
	if (toInviteTo->getUser(nickname) != NULL)
		return (sendMessageToClient(client.getSocketFD(), ERR_USERONCHANNEL(getName(), client.getClientName(), nickname, channelName)), 1);

	if (cmdNumber < command.size())
		return (std::cerr << "TOO MANY PARAMETERS" << std::endl, 1);

	//CHECK CHANNEL MODE (INVITE ONLY)	// ERR_CHANOPRIVSNEEDED (482)

	toBeInvited = getClient(nickname);
	if (toBeInvited == NULL)
		return (sendMessageToClient(client.getSocketFD(), ERR_NOSUCHNICK(getName(), client.getClientName(), nickname)), 1);


	// RPL_INVITING (341)
	// When the invite is successful, the server MUST send a RPL_INVITING numeric to the command issuer,
	// and an INVITE message, with the issuer as <source>?????????, to the target user. Other channel members SHOULD NOT be notified.

	sendMessageToClient(client.getSocketFD(), RPL_INVITING(getName(), client.getClientName(), nickname, channelName));
	//toBeInvited needs to get reassigend by pais function Server::getUser(nickname);
	//MSG_INVITE

	//CHECK IF TOPIC OR NOT
	sendMessageToClient(toBeInvited->getSocketFD(), RPL_TOPIC(getName(), client.getClientName(), channelName, toInviteTo->getTopic()));

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
		return (sendMessageToClient(client.getSocketFD(), ERR_NEEDMOREPARAMS(getName(), client.getClientName(), "TOPIC")), 1);
	
	//CHECK IF CHANNEL EXISTS
	channelName = command[cmdNumber++];
	toTakeTopicFrom = getChannel(channelName);
	if (toTakeTopicFrom == NULL)
		return (sendMessageToClient(client.getSocketFD(), ERR_NOSUCHCHANNEL(getName(), client.getClientName(), channelName)), 1);

	//CHECK THE TOPIC
	if (command.size() == 2)
	{
		if (toTakeTopicFrom->getTopic().empty() == true)
			sendMessageToClient(client.getSocketFD(), RPL_NOTOPIC(getName(), client.getNickname(), channelName));
		else
			sendMessageToClient(client.getSocketFD(), RPL_TOPIC(getName(), client.getClientName(), channelName, toTakeTopicFrom->getTopic()));
	}
	else if (command.size() == 3)
	{
		if (command[cmdNumber][0] == ':' && command[cmdNumber].size() == 1)
		{
			toTakeTopicFrom->setTopic("");
			for (size_t i = 0; i < toTakeTopicFrom->getChannelUsers().size(); i++)
				sendMessageToClient(client.getSocketFD(), RPL_NOTOPIC(getName(), client.getNickname(), channelName));
		}
		else
		{
			toTakeTopicFrom->setTopic(command[cmdNumber].substr(1, command[cmdNumber].length() -1));
			for (size_t i = 0; i < toTakeTopicFrom->getChannelUsers().size(); i++)
				sendMessageToClient(client.getSocketFD(), RPL_TOPIC(getName(), client.getClientName(), channelName, toTakeTopicFrom->getTopic()));
		}
	}
	else
		return (std::cerr << "TOO MANY PARAMETERS" << std::endl, -1);
	return (0);
}

int	Server::mode(Client &client, std::vector<std::string> command, size_t cmdNumber)
{
	std::string	channelName, modeString;
	Channel		*toChangeMode;
	bool		doEnable = true;

	//CHECK NUMBER OF NECESSARY PARAMETERS
	if (command.size() < 2)
		return (sendMessageToClient(client.getSocketFD(), ERR_NEEDMOREPARAMS(getName(), client.getClientName(), "MODE")), 1);

	//CHECK IF CHANNEL EXISTS
	channelName = command[cmdNumber++];
	toChangeMode = getChannel(channelName);
	if (toChangeMode == NULL)
		return (sendMessageToClient(client.getSocketFD(), ERR_NOSUCHCHANNEL(getName(), client.getClientName(), channelName)), 1);
	if (command.size() == 2)
	{
		//RPL_CREATIONTIME (329)
		return (sendMessageToClient(client.getSocketFD(), RPL_CHANNELMODEIS(getName(), client.getNickname(), channelName, toChangeMode->getModes(), toChangeMode->getModeArguments())), 1);
	}
	else
	{
		//CHECK FOR CHANNEL PERMISSIONS
		if (toChangeMode->isOperator(&client) == false)
			return (sendMessageToClient(client.getSocketFD(), ERR_CHANOPRIVSNEEDED(getName(), client.getNickname(), channelName)), 1);

		modeString = command[cmdNumber++];
		for (size_t i = 0; i < modeString.length(); i++)
		{
			if (modeString[i] == '+' || modeString[i] == '-')
			{
				if (modeString[i] == '-')
					doEnable = false;
				else
					doEnable = true;
				continue ;
			}
			else if ((modeString[i] == 'k' || modeString[i] == 'o' || modeString[i] == 'l') && !(cmdNumber < command.size()))
				continue ;
			else if (modeString[i] == 'i')
				toChangeMode->setMode('i', "", doEnable);
			else if (modeString[i] == 'o')
					toChangeMode->setOperator(command[cmdNumber++], doEnable);
			else if (modeString[i] == 't')
				toChangeMode->setMode('t', "", doEnable);
			else if (modeString[i] == 'l')
					toChangeMode->setMode('l', command[cmdNumber++], doEnable);
			else if (modeString[i] == 'k')
					toChangeMode->setMode('k', command[cmdNumber++], doEnable);
			else
				sendMessageToClient(client.getSocketFD(), ERR_UNKNOWNMODE(getName(), client.getClientName(), modeString[i]));
			sendMessageToChannel(&client, toChangeMode, RPL_CHANNELMODEIS(getName(), client.getNickname(), channelName, toChangeMode->getModes(), toChangeMode->getModeArguments()));
		}
	}
	
	// ERR_NOTONCHANNEL (442)

	return (0);
}


void	Server::testAllNumericReplies(int clientFD, Client &client)
{
	
	//sendMessageToClient(clientFD, MSG_NICK(client.getFullIdentifier(), "shaintha2"));
	sendMessageToClient(clientFD, MSG_NICK(client.getNickname(), "shaintha3"));
	//sendMessageToClient(clientFD, MSG_NICK(client.getFullIdentifier(), "shaintha4"));
	// sendMessageToClient(clientFD, RPL_WELCOME(getName(), client.getNickname(), "shaintha", "admin", "localhost")); //WORKS
	// sendMessageToClient(clientFD, RPL_CHANNELMODEIS(getName(), client.getNickname(), "#test", "tkl", "hello_World 10")); //WORKS
	// sendMessageToClient(clientFD, RPL_NOTOPIC(getName(), client.getNickname(), "#test"));
	// sendMessageToClient(clientFD, RPL_TOPIC(getName(), client.getNickname(), "#test", "HOW TO PASS FT_IRC")); //WORKS
	// sendMessageToClient(clientFD, RPL_INVITING(getName(), client.getNickname(), "juitz", "#test")); //WORKS
	// sendMessageToClient(clientFD, RPL_NAMREPLY(getName(), client.getNickname(), "=", "#test", "shaintha juitz")); //WORKS
	// sendMessageToClient(clientFD, RPL_ENDOFNAMES(getName(), client.getNickname(), "#test"));
	// sendMessageToClient(clientFD, ERR_NOSUCHNICK(getName(), client.getNickname(), "juitz"));
	// sendMessageToClient(clientFD, ERR_NOSUCHCHANNEL(getName(), client.getNickname(), "#test"));
	// sendMessageToClient(clientFD, ERR_CANNOTSENDTOCHAN(getName(), client.getNickname(), "#test"));
	// sendMessageToClient(clientFD, ERR_TOOMANYCHANNELS(getName(), client.getNickname(), "#test"));
	// sendMessageToClient(clientFD, ERR_NORECIPIENT(getName(), client.getNickname(), "PRIVMSG")); //WORKS
	// sendMessageToClient(clientFD, ERR_NOTEXTTOSEND(getName(), client.getNickname())); //WORKS
	// sendMessageToClient(clientFD, ERR_INPUTTOOLONG(getName(), client.getNickname())); //WORKS
	// sendMessageToClient(clientFD, ERR_UNKNOWNCOMMAND(getName(), client.getNickname(), "LOL"));
	// sendMessageToClient(clientFD, ERR_NONICKNAMEGIVEN(getName(), client.getNickname()));
	// sendMessageToClient(clientFD, ERR_ERRONEUSNICKNAME(getName(), client.getNickname(), "juitz")); //WORKS
	// sendMessageToClient(clientFD, ERR_NICKNAMEINUSE(getName(), client.getNickname(), "juitz")); //WORKS
	// sendMessageToClient(clientFD, ERR_NICKCOLLISION(getName(), client.getNickname(), "juitz", "user", "host"));
	// sendMessageToClient(clientFD, ERR_USERNOTINCHANNEL(getName(), client.getNickname(), "juitz", "#test"));
	// sendMessageToClient(clientFD, ERR_NOTONCHANNEL(getName(), client.getNickname(), "#test"));
	// sendMessageToClient(clientFD, ERR_USERONCHANNEL(getName(), client.getNickname(), "juitz", "#test"));
	// sendMessageToClient(clientFD, ERR_NOTREGISTERED(getName(), client.getNickname())); //WORKS
	// sendMessageToClient(clientFD, ERR_NEEDMOREPARAMS(getName(), client.getNickname(), "PASS"));
	// sendMessageToClient(clientFD, ERR_ALREADYREGISTERED(getName(), client.getNickname())); //WORKS
	// sendMessageToClient(clientFD, ERR_PASSWDMISMATCH(getName(), client.getNickname())); //WORKS
	// sendMessageToClient(clientFD, ERR_YOUREBANNEDCREEP(getName(), client.getNickname())); //WORKS
	// sendMessageToClient(clientFD, ERR_CHANNELISFULL(getName(), client.getNickname(), "#test")); //WORKS
	// sendMessageToClient(clientFD, ERR_UNKNOWNMODE(getName(), client.getNickname(), "p"));
	// sendMessageToClient(clientFD, ERR_INVITEONLYCHAN(getName(), client.getNickname(), "#test")); //WORKS
	// sendMessageToClient(clientFD, ERR_BANNEDFROMCHAN(getName(), client.getNickname(), "#test")); //WORKS
	// sendMessageToClient(clientFD, ERR_BADCHANNELKEY(getName(), client.getNickname(), "#test")); //WORKS
	// sendMessageToClient(clientFD, ERR_BADCHANMASK(getName(), client.getNickname(), "test"));
	// sendMessageToClient(clientFD, ERR_CHANOPRIVSNEEDED(getName(), client.getNickname(), "#test"));
	// sendMessageToClient(clientFD, ERR_INVALIDKEY(getName(), client.getNickname(), "#test"));
	// sendMessageToClient(clientFD, ERR_INVALIDMODEPARAM(getName(), client.getNickname(), "#test", "l", "abc", "NO numbees"));
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
