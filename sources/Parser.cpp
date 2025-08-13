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
		executeCommand(client, command, input);
	if (command[0].compare("QUIT") != 0 && client.getState() < REGISTERED && client.getNickname() != "*" && client.getUsername() != "*")
	{
		client.setState(REGISTERED);
		sendMessageToClient(client.getSocketFD(), RPL_WELCOME(getName(), client.getNickname(), client.getNickname(), client.getUsername(), client.getHostname()));
	}
}

void    Server::parseStringToVector(std::string input, std::vector<std::string> *vector, const char *delimiters)
{
	for (char* token = std::strtok((char *)input.c_str(), delimiters); token; token = std::strtok(NULL, delimiters))
		vector->push_back(token);
}

size_t	Server::getInputPosition(std::string &input, size_t numberOfArguments)
{
	size_t pos = 0;

	for (size_t i = 0; i < numberOfArguments; i++)
	{
		while (input[pos] == ' ')
			input[pos++];
		if (i + 1 == numberOfArguments)
			return (pos);
		while (input[pos] != ' ')
			input[pos++];
	}
	return (pos);
}

void	Server::executeCommand(Client &client, std::vector<std::string> command, std::string &input)
{
	if (command[0].compare("PASS") == 0)
		pass(client, command, 1);
	else if (command[0].compare("NICK") == 0)
		nick(client, command, 1);
	else if (command[0].compare("USER") == 0)
		user(client, command, input, 1);
	else if (command[0].compare("JOIN") == 0)
		join(client, command, 1);
	else if (command[0].compare("PRIVMSG") == 0)
		privMsg(client, command, input, 1);
	else if (command[0].compare("PART") == 0)
		part(client, command, input, 1);
	else if (command[0].compare("KICK") == 0)
		kick(client, command, input, 1);
	else if (command[0].compare("INVITE") == 0)
		invite(client, command, 1);
	else if (command[0].compare("TOPIC") == 0)
		topic(client, command, input, 1);
	else if (command[0].compare("MODE") == 0)
		mode(client, command, 1);
	else if (command[0].compare("QUIT") == 0)
		quit(client, command, 1);
	else if (command[0].compare("WHO") == 0)
		return ;
	else if (command[0].compare("TEST") == 0)
		testAllNumericReplies(client.getSocketFD(), client);
	else
		sendMessageToClient(client.getSocketFD(), ERR_UNKNOWNCOMMAND(getName(), client.getClientName(), command[0]));
}

int	Server::pass(Client &client, std::vector<std::string> command, size_t cmdNumber)
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
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
		sendMessageToClient(it->second->getSocketFD(), MSG_NICK(client.getNickname(), command[cmdNumber]));
	client.setNick(command[cmdNumber]);
	return (0);
}

int	Server::user(Client &client, std::vector<std::string> command, std::string &input, size_t cmdNumber)
{
	std::string	realname;

	if (command.size() < 5)
		return (sendMessageToClient(client.getSocketFD(), ERR_NEEDMOREPARAMS(getName(), client.getClientName(), "USER")), 1);
	else if (client.getState() >= REGISTERED)
		return (sendMessageToClient(client.getSocketFD(), ERR_ALREADYREGISTERED(getName(), client.getClientName())), 1);

	//  DO CHECKS FOR 0 an *

	cmdNumber = 4;
	if (command[cmdNumber][0] != ':')
		realname = command[cmdNumber];
	else
		realname = input.substr(getInputPosition(input, 5) + 1);
	client.setUser(command[cmdNumber], 0, '*', realname);
	return (0);
}

std::string uintToString(unsigned int value)
{
    std::ostringstream oss;
    oss << value;
    return (oss.str());
}

int	Server::join(Client &client, std::vector<std::string> command, size_t cmdNumber) //JOIN <channel>{,<channel>} [<key>{,<key>}]
{
	std::vector<std::string>	channelNames, keyNames;
	Channel						*toJoinTo;

	if (command.size() < 2)
		return (sendMessageToClient(client.getSocketFD(), ERR_NEEDMOREPARAMS(getName(), client.getClientName(), "JOIN")), 1);
	
	//Parsing the command argument into channel name(s) stored in a vector
	parseStringToVector(command[cmdNumber++], &channelNames, ",");
	if (channelNames.size() == 0)
		return (1);

	//Parsing the command argument into user name(s) stored in a vector
	if (cmdNumber < command.size())
		parseStringToVector(command[cmdNumber++], &keyNames, ",");

	for (size_t i = 0; i < channelNames.size(); i++)
	{
		// if (channelNames.size() == 1 && channelNames[i].compare("0") == 0)
		// {
		// 	//PART WIH ALL CHANNELS
		// }	
		toJoinTo = getChannel(channelNames[i]);
		if (toJoinTo == NULL && _channels.size() >= MAX_CHAN_NUM)
		{
			sendMessageToClient(client.getSocketFD(), ERR_NOSUCHCHANNEL(getName(), client.getClientName(), channelNames[i]));
			continue ;
		}
		else if (channelNames[i][0] != '#')
		{
			sendMessageToClient(client.getSocketFD(), ERR_BADCHANMASK(getName(), client.getClientName(), channelNames[i]));
			continue ;
		}
		else if (client.getChannelNumber() >= CHANLIMIT)
		{
			sendMessageToClient(client.getSocketFD(), ERR_TOOMANYCHANNELS(getName(), client.getClientName(), channelNames[i]));
			continue ;
		}
		else if (toJoinTo == NULL)
			createChannel(channelNames[i], client);
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
			else if (toJoinTo->hasMode('l') == true)
			{
				if (toJoinTo->getChannelUsers().size() + toJoinTo->getOperators().size() >= toJoinTo->getUserLimit())
				{
					return (sendMessageToClient(client.getSocketFD(), ERR_CHANNELISFULL(getName(), client.getNickname(), channelNames[i])), 1);
				}
			}
			else if (toJoinTo->hasMode('i') == true)
			{
				sendMessageToClient(client.getSocketFD(), ERR_INVITEONLYCHAN(getName(), client.getNickname(), channelNames[i]));
				continue ;
			}
			toJoinTo->addUser(&client);
			sendMessageToClient(client.getSocketFD(), MSG_JOIN(client.getNickname(), channelNames[i]));
			if (toJoinTo->getTopic().empty() == true)
				sendMessageToClient(client.getSocketFD(), RPL_NOTOPIC(getName(), client.getNickname(), channelNames[i]));
			else
				sendMessageToClient(client.getSocketFD(), RPL_TOPIC(getName(), client.getNickname(), channelNames[i], toJoinTo->getTopic()));
			sendMessageToChannel(toJoinTo, RPL_NAMREPLY(getName(), client.getNickname(), "=", channelNames[i], toJoinTo->getNamesOfChannelMembers()));
			sendMessageToChannel(toJoinTo, RPL_ENDOFNAMES(getName(), client.getNickname(), channelNames[i]));
		}
	}
	return (0);
}

int	Server::privMsg(Client &client, std::vector<std::string> command, std::string input, size_t cmdNumber)
{
	std::vector<std::string> targets;
	std::string message = "";

	if (command.size() < 2)
		return (sendMessageToClient(client.getSocketFD(), ERR_NEEDMOREPARAMS(getName(), client.getNickname(), "PRIVMSG")), 1);

	parseStringToVector(command[cmdNumber++], &targets, ",");
	if (targets.size() == 0)
		return (1);
		
	if (cmdNumber >= command.size())
	return (sendMessageToClient(client.getSocketFD(), ERR_NOTEXTTOSEND(getName(), client.getClientName())), 1);
	
	if (command[cmdNumber][0] != ':')
		message = command[cmdNumber];
	else
		message = input.substr(getInputPosition(input, 3) + 1);

	for (size_t i = 0; i < targets.size(); i++)
	{
		if (targets[i][0] == '#')
		{
			Channel *channel = getChannel(targets[i]);
			if (channel == NULL)
			{
				sendMessageToClient(client.getSocketFD(), ERR_NOSUCHCHANNEL(getName(), client.getClientName(), targets[i]));
				continue ;
			}
			sendMessageToChannel(&client, channel, MSG_PRIVMSG(client.getFullIdentifier(), targets[i], message));
		}
		else
		{
			Client *targetUser = getClient(targets[i]);
			if (targetUser == NULL)
			{
				sendMessageToClient(client.getSocketFD(), ERR_NOSUCHNICK(getName(), client.getClientName(), targets[i]));
				continue ;
			}
			sendMessageToClient(targetUser->getSocketFD(), MSG_PRIVMSG(client.getFullIdentifier(), targets[i], message));
		}
	}
	return (0);
}

int Server::part(Client &client, std::vector<std::string> command, std::string &input, size_t cmdNumber)
{
    std::vector<std::string>	channelNames;
    std::string 				comment = "";
	Channel						*toPartFrom;
    
    if (command.size() < 2)
        return (sendMessageToClient(client.getSocketFD(), ERR_NEEDMOREPARAMS(getName(), client.getClientName(), "PART")), 1);
    
    parseStringToVector(command[cmdNumber++], &channelNames, ",");
    if (channelNames.size() == 0)
        return (1);
    
    if (cmdNumber < command.size())
    {
        if (command[cmdNumber][0] != ':')
            comment = command[cmdNumber];
		else
			comment = input.substr(getInputPosition(input, 3) + 1);
        cmdNumber++;
    }
    for (size_t i = 0; i < channelNames.size(); i++)
    {
        if (channelNames[i][0] != '#')
        {
            sendMessageToClient(client.getSocketFD(), ERR_BADCHANMASK(getName(), client.getClientName(), channelNames[i]));
            continue;
        }
        toPartFrom = getChannel(channelNames[i]);
        if (toPartFrom == NULL)
        {
            sendMessageToClient(client.getSocketFD(), ERR_NOSUCHCHANNEL(getName(), client.getClientName(), channelNames[i]));
            continue;
        }
        else if (toPartFrom->getUser(client.getNickname()) == NULL)
        {
            sendMessageToClient(client.getSocketFD(), ERR_NOTONCHANNEL(getName(), client.getNickname(), channelNames[i]));
            continue;
        }

        toPartFrom->removeUser(&client);
		client.setChannelNumber(-1);
        if (comment.empty() == true)
            sendMessageToClient(client.getSocketFD(), MSG_PART(client.getClientName(), channelNames[i]));
        else
            sendMessageToClient(client.getSocketFD(), MSG_PART_WITH_COMMENT(client.getClientName(), channelNames[i], comment));
        
        sendMessageToChannel(&client, toPartFrom, MSG_PART(client.getClientName(), channelNames[i]));
		if (toPartFrom->getChannelUsers().size() + toPartFrom->getOperators().size() == 0)
		{
			removeChannel(toPartFrom);
			//delete toPartFrom;
		}
    }
    return (0);
}

int	Server::kick(Client &client, std::vector<std::string> command, std::string &input, size_t cmdNumber) //KICK <channel> <user>[,<user>,...] [:<comment>]
{
	std::vector<std::string>	users;
	std::string					channelName, comment = "";
	Channel						*toKickFrom;
	Client						*toBeKicked;

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
	else if (toKickFrom->getUser(client.getNickname()) == NULL)
		return (sendMessageToClient(client.getSocketFD(), ERR_NOTONCHANNEL(getName(), client.getClientName(), channelName)), 1);
	else if (toKickFrom->isOperator(&client) == false)
		return (sendMessageToClient(client.getSocketFD(), ERR_CHANOPRIVSNEEDED(getName(), client.getClientName(), channelName)), 1);

	//Parsing the command argument into user name(s) stored in a vector
	parseStringToVector(command[cmdNumber++], &users, ",");
	if (users.size() == 0)
		return (1);

	//Check for a KICK message (that starts with ':')
	if (cmdNumber < command.size())
	{
		if (command[cmdNumber][0] != ':')
            comment = command[cmdNumber];
		else
			comment = input.substr(getInputPosition(input, 4) + 1);
	}

	//Looping through the channels and users to be toBeKicked
	for (size_t i = 0; i < users.size(); i++)
	{
		toBeKicked = toKickFrom->getUser(users[i]);
		if (toBeKicked == NULL)
		{
			sendMessageToClient(client.getSocketFD(), ERR_USERNOTINCHANNEL(getName(), client.getClientName(), users[i], channelName));
			continue ;
		}
		toKickFrom->removeUser(toBeKicked);
		toBeKicked->setChannelNumber(-1);
		if (comment.empty() == true)
		{
			sendMessageToClient(toBeKicked->getSocketFD(), MSG_KICK(client.getClientName(), channelName, toBeKicked->getNickname()));
			sendMessageToChannel(toKickFrom, MSG_KICK(client.getClientName(), channelName, toBeKicked->getNickname()));
		}
		else
		{
			sendMessageToClient(toBeKicked->getSocketFD(), MSG_KICK_WITH_COMMENT(client.getClientName(), channelName, toBeKicked->getNickname(), comment));
			sendMessageToChannel(toKickFrom, MSG_KICK_WITH_COMMENT(client.getClientName(), channelName, toBeKicked->getNickname(), comment));
		}
		sendMessageToChannel(toKickFrom, RPL_NAMREPLY(getName(), toBeKicked->getNickname(), "=", channelName, toKickFrom->getNamesOfChannelMembers()));
		sendMessageToChannel(toKickFrom, RPL_ENDOFNAMES(getName(), toBeKicked->getNickname(), channelName));
		if (toKickFrom->getOperators().size() + toKickFrom->getChannelUsers().size() == 0)
			removeChannel(toKickFrom);
	}
	return (0);
}

int	Server::invite(Client &client, std::vector<std::string> command, size_t cmdNumber) //INVITE <nickname> <channel>
{
	std::string	nickname, channelName;
	Channel		*toInviteTo;
	Client		*toBeInvited;

	//CHECK NUMBER OF NECESSARY PARAMETERS
	if (command.size() < 3)
		return (sendMessageToClient(client.getSocketFD(), ERR_NEEDMOREPARAMS(getName(), client.getClientName(), "INVITE")), 1);

	nickname = command[cmdNumber++];
	//CHECK IF CHANNEL EXISTS
	channelName = command[cmdNumber++];
	toInviteTo = getChannel(channelName);
	if (toInviteTo == NULL)
		return (sendMessageToClient(client.getSocketFD(), ERR_NOSUCHCHANNEL(getName(), client.getClientName(), channelName)), 1);

	else if (toInviteTo->hasMode('l') == true)
	{
		if (toInviteTo->getChannelUsers().size() + toInviteTo->getOperators().size() >= toInviteTo->getUserLimit())
		{
			return (sendMessageToClient(client.getSocketFD(), ERR_CHANNELISFULL(getName(), client.getNickname(), channelName)), 1);
		}
	}
	//CHECK IF COMMAND ISSUER PART OF THAT CHANNEL
	else if (toInviteTo->getUser(client.getNickname()) == NULL)
		return (sendMessageToClient(client.getSocketFD(), ERR_NOTONCHANNEL(getName(), client.getClientName(), channelName)), 1);

	//CHECK CHANNEL MODE (INVITE ONLY) & PERMISSIONS
	else if (toInviteTo->hasMode('i') == true && toInviteTo->isOperator(&client) == false)
		return (sendMessageToClient(client.getSocketFD(), ERR_CHANOPRIVSNEEDED(getName(), client.getClientName(), channelName)), 1);
	
	//CHECK IF CLIENT IS PART OF THAT CHANNEL
	else if (toInviteTo->getUser(nickname) != NULL)
		return (sendMessageToClient(client.getSocketFD(), ERR_USERONCHANNEL(getName(), client.getClientName(), nickname, channelName)), 1);

	toBeInvited = getClient(nickname);
	if (toBeInvited == NULL)
		return (sendMessageToClient(client.getSocketFD(), ERR_NOSUCHNICK(getName(), client.getClientName(), nickname)), 1);
	sendMessageToClient(client.getSocketFD(), RPL_INVITING(getName(), client.getClientName(), nickname, channelName));
	sendMessageToClient(client.getSocketFD(), MSG_INVITE(client.getClientName(), nickname, channelName));
	toInviteTo->addUser(toBeInvited);
	sendMessageToClient(toBeInvited->getSocketFD(), MSG_JOIN(toBeInvited->getNickname(), channelName));
	if (toInviteTo->getTopic().empty() == true)
		sendMessageToClient(toBeInvited->getSocketFD(), RPL_NOTOPIC(getName(), toBeInvited->getNickname(), channelName));
	else
		sendMessageToClient(toBeInvited->getSocketFD(), RPL_TOPIC(getName(), toBeInvited->getNickname(), channelName, toInviteTo->getTopic()));
	sendMessageToChannel(toInviteTo, RPL_NAMREPLY(getName(), toBeInvited->getNickname(), "=", channelName, toInviteTo->getNamesOfChannelMembers()));
	sendMessageToChannel(toInviteTo, RPL_ENDOFNAMES(getName(), toBeInvited->getNickname(), channelName));
	return (0);
}

int	Server::topic(Client &client, std::vector<std::string> command, std::string &input, size_t cmdNumber) //TOPIC <channel> [<topic>]
{
	std::string	channelName, topic;
	Channel		*toTakeTopicFrom;

	if (command.size() < 2)
		return (sendMessageToClient(client.getSocketFD(), ERR_NEEDMOREPARAMS(getName(), client.getClientName(), "TOPIC")), 1);
	channelName = command[cmdNumber++];
	toTakeTopicFrom = getChannel(channelName);
	if (toTakeTopicFrom == NULL)
		return (sendMessageToClient(client.getSocketFD(), ERR_NOSUCHCHANNEL(getName(), client.getClientName(), channelName)), 1);
	else if (toTakeTopicFrom->getUser(client.getNickname()) == NULL)
		return (sendMessageToClient(client.getSocketFD(), ERR_NOTONCHANNEL(getName(), client.getClientName(), channelName)), 1);
	
	if (command.size() == 2)
	{
		if (toTakeTopicFrom->getTopic().empty() == true)
			sendMessageToClient(client.getSocketFD(), RPL_NOTOPIC(getName(), client.getNickname(), channelName));
		else
			sendMessageToClient(client.getSocketFD(), RPL_TOPIC(getName(), client.getClientName(), channelName, toTakeTopicFrom->getTopic()));
	}
	else
	{
		topic = command[cmdNumber++];
		if (toTakeTopicFrom->isOperator(&client) == false)
			return (sendMessageToClient(client.getSocketFD(), ERR_CHANOPRIVSNEEDED(getName(), client.getClientName(), channelName)), 1);
		else if (topic[0] == ':' && topic.length() == 1)
			toTakeTopicFrom->setTopic("");
		else if (topic[0] == ':')
			toTakeTopicFrom->setTopic(input.substr(getInputPosition(input, 3) + 1));
		else
			toTakeTopicFrom->setTopic(topic);
		if (toTakeTopicFrom->getTopic().empty() == false)
			sendMessageToChannel(toTakeTopicFrom, MSG_TOPIC(client.getClientName(), channelName, toTakeTopicFrom->getTopic()));
		else
			sendMessageToChannel(toTakeTopicFrom, MSG_CLEAR_TOPIC(client.getClientName(), channelName));
	}
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
	else if (toChangeMode->getUser(client.getNickname()) == NULL)
		return (sendMessageToClient(client.getSocketFD(), ERR_NOTONCHANNEL(getName(), client.getClientName(), channelName)), 1);
	else if (command.size() == 2)
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
			else if ((modeString[i] == 'k' || modeString[i] == 'o' || (modeString[i] == 'l' && doEnable == true)) && !(cmdNumber < command.size()))
				continue ;
			else if (modeString[i] == 'i')
				toChangeMode->setMode('i', "", doEnable);
			else if (modeString[i] == 't')
				toChangeMode->setMode('t', "", doEnable);
			else if (modeString[i] == 'l' && doEnable == true)
				toChangeMode->setMode('l', command[cmdNumber++], doEnable);
			else if (modeString[i] == 'l' && doEnable == true)
				toChangeMode->setMode('l', "", doEnable);
			else if (modeString[i] == 'l' && doEnable == false)
				toChangeMode->setMode('l', "", doEnable);
			else if (modeString[i] == 'k')
				toChangeMode->setMode('k', command[cmdNumber++], doEnable);
			else if (modeString[i] == 'k' && doEnable == false)
				toChangeMode->setMode('k', "", doEnable);
			else if (modeString[i] == 'o')
				toChangeMode->setOperator(command[cmdNumber++], doEnable);
			else
				sendMessageToClient(client.getSocketFD(), ERR_UNKNOWNMODE(getName(), client.getClientName(), modeString[i]));
			sendMessageToChannel(&client, toChangeMode, RPL_CHANNELMODEIS(getName(), client.getNickname(), channelName, toChangeMode->getModes(), toChangeMode->getModeArguments()));
		}
		sendMessageToChannel(toChangeMode, MSG_MODE(client.getClientName(), channelName, toChangeMode->getModes(), toChangeMode->getModeArguments()));
	}
	MSG_MODE(client.getClientName(),channelName, toChangeMode->getModes(), toChangeMode->getModeArguments());

	return (0);
}

int Server::quit(Client &client, std::vector<std::string> command, size_t cmdNumber)
{
	std::string reason = command.size() > cmdNumber ? command[cmdNumber] : "";
	int i = 0;
	while (_pollfds[i].fd != client.getSocketFD())
		i++;
	std::cout << GRAY << "Disconnect client with fd: " << client.getSocketFD() << " | Reason: " << reason << DEFAULT << std::endl;
	handleClientDisconnections(i);
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

void	Server::printVector(std::vector<std::string> vector)
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
