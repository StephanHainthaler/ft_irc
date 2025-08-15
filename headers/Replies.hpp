/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shaintha <shaintha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 15:11:53 by shaintha          #+#    #+#             */
/*   Updated: 2025/08/05 15:11:57 by shaintha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPLIES_HPP
# define REPLIES_HPP

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//COMMAND REPLIES

//":<source>/<oldNick>!<user>@<host> NICK :<newNick>"
# define MSG_NICK(source, newNick)		                        						":" + source + " NICK :" + newNick

//":<source>/<nick>!<user>@<host> JOIN :<channel>"
# define MSG_JOIN(source, channel)														":" + source + " JOIN :" + channel

//":<client>/<nick>!<user>@<host> PART :<channel>"
# define MSG_PART(source, channel)														":" + source + " PART " + channel
# define MSG_PART_WITH_COMMENT(source, channel, comment)								":" + source + " PART " + channel + " " + comment

//":<source> PRIVMSG <channel/user> :<message>
# define MSG_PRIVMSG(source, target, messsage)                 							":" + source + " PRIVMSG " + target + " :" + message

//":<source> KICK <channel> <nick> [:<comment>]
# define MSG_KICK(source, channel, nick)												":" + source + " KICK " + channel + " " + nick
# define MSG_KICK_WITH_COMMENT(source, channel, nick, comment)							":" + source + " KICK " + channel + " " + nick + " :" + comment

//:<source> INVITE <nick> <channel>
# define MSG_INVITE(source, nick, channel)												":" + source + " INVITE " + nick + " " + channel

//":<source> TOPIC <channel> [:<newTopic>]"
# define MSG_TOPIC(source, channel, newTopic)											":" + source + " TOPIC " + channel + " :" + newTopic
# define MSG_CLEAR_TOPIC(source, channel)												":" + source + " TOPIC " + channel + " :"

//":<source> MODE <channel> <modeString> <modeArguments>"
# define MSG_MODE(source, channel, plusminus, modeString)                               ":" + source + " MODE " + channel + " " + plusminus + modeString
# define MSG_MODE_WITH_ARGS(source, channel, plusminus, modeString, modeArguments)      ":" + source + " MODE " + channel + " " + plusminus + modeString + " " + modeArguments

//":<source> QUIT <channel> <modeString> <modeArguments>"
# define MSG_QUIT(source)																":" + source + " QUIT :Leaving"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//NUMERIC REPLIES 

//Distinct from a normal message, a numeric reply MUST contain a <source> and use a three-digit numeric as the command. 
//A numeric reply SHOULD contain the target of the reply as the first parameter of the message.
//A numeric reply is not allowed to originate from a client.

//":<source> 001 <client> :Welcome to the <networkname> Network, <nick>[!<user>@<host>]"
# define RPL_WELCOME(source, client, nick, user, host)  						":" + source + " 001 " + client + " :Welcome to the Internet Relay Chat Network, " + nick + "!" + user + "@" + host

//":<source> 324 <client> <channel> <modestring> <mode arguments>..."
# define RPL_CHANNELMODEIS(source, client, channel, modeString, modeArguments)	":" + source + " 324 " + client + " " + channel + " " + modeString + " " + modeArguments

//":<source> 331 <client> <channel> :No topic is set"
# define RPL_NOTOPIC(source, client, channel)									":" + source + " 331 " + client + " " + channel + " :No topic is set"

//":<source> 332 <client> <channel> :<topic>"
# define RPL_TOPIC(source, client, channel, topic)								":" + source + " 332 " + client + " " + channel + " :" + topic

//":<source> 341 <client> <nick> <channel>"
# define RPL_INVITING(source, client, nick, channel)							":" + source + " 341 " + client + " " + nick + " " + channel

//":<source> 353 <client> <symbol> <channel> :[prefix]<nick>{ [prefix]<nick>}"
# define RPL_NAMREPLY(source, client, symbol, channel, channelMembers)			":" + source + " 353 " + client + " " + symbol + " " + channel + " :" + channelMembers

//":<source> 366 <client> <channel> :End of /NAMES list"
# define RPL_ENDOFNAMES(source, client, channel)								":" + source + " 366 " + client + " " + channel + " :End of /NAMES list"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//NUMERIC ERRORS

//":<source> 401 <client> <nickname> :No such nick/channel"
# define ERR_NOSUCHNICK(source, client, nick)					":" + source + " 401 " + client + " " + nick + " :No such nick/channel"

//":<source> 403 <client> <channel> :No such channel"
# define ERR_NOSUCHCHANNEL(source, client, channel)				":" + source + " 403 " + client + " " + channel + " :No such channel"

//":<source> 404 <client> <channel> :Cannot send to channel"
# define ERR_CANNOTSENDTOCHAN(source, client, channel)			":" + source + " 404 " + client + " " + channel + " :Cannot send to channel"

//":<source> 405 <client> <channel> :You have joined too many channels"
# define ERR_TOOMANYCHANNELS(source, client, channel)			":" + source + " 405 " + client + " " + channel + " :You have joined too many channels"

//":<source> 411 <client> :No recipient given (<command>)"
# define ERR_NORECIPIENT(source, client, command) 				":" + source + " 411 " + client + " :No recipient given (" + command + ")"			

//":<source> 412 <client> :No text to send"
# define ERR_NOTEXTTOSEND(source, client) 						":" + source + " 412 " + client + " :No text to send"

//":<source> 417 <client> :Input line was too long"
# define ERR_INPUTTOOLONG(source, client)						":" + source + " 417 " + client + " :Input line was too long"

//":<source> 421 <client> <command> :Unknown command"
# define ERR_UNKNOWNCOMMAND(source, client, command)			":" + source + " 421 " + client + " " + command + " :Unknown command"

//":<source> 431 <client> :No nickname given"
# define ERR_NONICKNAMEGIVEN(source, client)					":" + source + " 431 " + client + " :No nickname given"

//":<source> 432 <client> <nick> :Erroneus nickname"
# define ERR_ERRONEUSNICKNAME(source, client, nick)				":" + source + " 432 " + client + " " + nick + " :Erroneus nickname"

//":<source> 433 <client> <nick> :Nickname is already in use"
# define ERR_NICKNAMEINUSE(source, client, nick)				":" + source + " 433 " + client + " " + nick + " :Nickname is already in use"

//":<source> 436 <client> <nick> :Nickname collision KILL from <user>@<host>"
# define ERR_NICKCOLLISION(source, client, nick, user, host)	":" + source + " 436 " + client + " " + nick + " :Nickname collision KILL from " + user + "@" + host

//":<source> 441 <client> <nick> <channel> :They aren't on that channel"
# define ERR_USERNOTINCHANNEL(source, client, nick, channel)	":" + source + " 441 " + client + " " + nick + " " + channel + " :They aren't on that channel"

//":<source> 442 <client> <channel> :You're not on that channel"
# define ERR_NOTONCHANNEL(source, client, channel)				":" + source + " 442 " + client + " " + channel + " :You're not on that channel"

//":<source> 443 <client> <nick> <channel> :is already on channel"
# define ERR_USERONCHANNEL(source, client, nick, channel)		":" + source + " 443 " + client + " " + nick + " " + channel + " :is already on channel"

//":<source> 451 <client> :You have not registered"
# define ERR_NOTREGISTERED(source, client)						":" + source + " 451 " + client + " :You have not registered"

//":<source> 461 <client> <command> :Not enough parameters"
# define ERR_NEEDMOREPARAMS(source, client, command)			":" + source + " 461 " + client + " " + command + " :Not enough parameters"

//":<source> 462 <client> :You may not reregister"
# define ERR_ALREADYREGISTERED(source, client)					":" + source + " 462 " + client + " :You may not reregister"

//":<source> 464 <client> :Password incorrect"
# define ERR_PASSWDMISMATCH(source, client)						":" + source + " 464 " + client + " :Password incorrect"

//":<source> 465 <client> :You are banned from this server."
# define ERR_YOUREBANNEDCREEP(source, client)					":" + source + " 465 " + client + " :You are banned from this server."

//":<source> 471 <client> <channel> :Cannot join channel (+l)"
# define ERR_CHANNELISFULL(source, client, channel)				":" + source + " 471 " + client + " " + channel + " :Cannot join channel (+l)"

//":<source> 472 <client> <modechar> :is unknown mode char to me"
# define ERR_UNKNOWNMODE(source, client, modeChar)				":" + source + " 472 " + client + " " + modeChar + " :is unknown mode char to me"

//":<source> 473 <client> <channel> :Cannot join channel (+i)"
# define ERR_INVITEONLYCHAN(source, client, channel)			":" + source + " 473 " + client + " " + channel + " :Cannot join channel (+i)"

//":<source> 474 <client> <channel> :Cannot join channel (+b)"
# define ERR_BANNEDFROMCHAN(source, client, channel)			":" + source + " 474 " + client + " " + channel + " :Cannot join channel (+b)"

//":<source> 475 <client> <channel> :Cannot join channel (+k)"
# define ERR_BADCHANNELKEY(source, client, channel)				":" + source + " 475 " + client + " " + channel + " :Cannot join channel (+k)"

//":<source> 476 <client> <channel> :Bad Channel Mask"
# define ERR_BADCHANMASK(source, client, channel)				":" + source + " 476 " + client + " " + channel + " :Bad Channel Mask"

//":<source> 482 <client> <channel> :You're not channel operator"
# define ERR_CHANOPRIVSNEEDED(source, client, channel)			":" + source + " 482 " + client + " " + channel + " :You're not channel operator"

//":<source> 525 <client> <target chan> :Key is not well-formed"
# define ERR_INVALIDKEY(source, client, channel)				":" + source + " 525 " + client + " " + channel + " :Key is not well-formed"

//":<source> 696 <client> <target chan/user> <mode char> <parameter> :<description>"
# define ERR_INVALIDMODEPARAM(source, client, channel, plusminus, modeChar, parameter, notes)	":" + source + " 696 " + client + " " + channel + " " + plusminus + modeChar + " " + parameter + " :" + notes 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
