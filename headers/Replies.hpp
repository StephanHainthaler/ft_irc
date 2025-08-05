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

//<client> JOIN :<channel>
# define MSG_JOIN(client, channel)				":" + client + " JOIN :" + channel						

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//NUMERIC REPLIES

//":<source> 001 <client> :Welcome to the <networkname> Network, <nick>[!<user>@<host>]"
# define RPL_WELCOME(source, client, nick, user, host)  				":" + source + " 001 " + client + " :Welcome to the Internet Relay Chat Network, " + nick + "!" + user + "@" + host

//"<client> <channel> <modestring> <mode arguments>..."
# define RPL_CHANNELMODEIS(client, channel, modeString, modeArguments)	":" + source + " 324 " + client + " " + channel + " " + modeString + " " + modeArguments

//"<client> <channel> :No topic is set"
# define RPL_NOTOPIC(client, channel)									": 331 " + client + " " + channel + " :No topic is set"

//"<client> <channel> :<topic>"
# define RPL_TOPIC(client, channel, topic)								": 332 " + client + " " + channel + " :" + topic

//"<client> <nick> <channel>"
# define RPL_INVITING(client, nick, channel)							": 341 " + client + " " + nick + " " + channel

//"<client> <symbol> <channel> :[prefix]<nick>{ [prefix]<nick>}"
# define RPL_NAMREPLY(client, symbol, channel, channelMembers)			": 353 " + client + " " + symbol + " " + channel + " :" + channelMembers

//"<client> <channel> :End of /NAMES list"
# define RPL_ENDOFNAMES(client, channel)								": 366 " + client + " " + channel + " :End of /NAMES list"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//NUMERIC ERRORS

//"<client> <nickname> :No such nick/channel"
# define ERR_NOSUCHNICK(client, nick)					": 401 " + client + " " + nick + " :No such nick/channel"

//"<client> <channel> :No such channel"
# define ERR_NOSUCHCHANNEL(client, channel)				": 403 " + client + " " + channel + " :No such channel"

//"<client> <channel> :Cannot send to channel"
# define ERR_CANNOTSENDTOCHAN(client, channel)			": 404 " + client + " " + channel + " :Cannot send to channel"

//"<client> <channel> :You have joined too many channels"
# define ERR_TOOMANYCHANNELS(client, channel)			": 405 " + client + " " + channel + " :You have joined too many channels"

//"<client> :No recipient given (<command>)"
# define ERR_NORECIPIENT(client, command) 				": 411 " + client + " :No recipient given (" + command + ")"			

//"<client> :No text to send"
# define ERR_NOTEXTTOSEND(client) 						": 412 " + client + " :No text to send"

//"<client> :Input line was too long"
# define ERR_INPUTTOOLONG(client)						": 417 " + client + " :Input line was too long"

//"<client> <command> :Unknown command"
# define ERR_UNKNOWNCOMMAND(client, command)			": 421 " + client + " " + command + " :Unknown command"

//"<client> :No nickname given"
# define ERR_NONICKNAMEGIVEN(client)					": 431 " + client + " :No nickname given"

//"<client> <nick> :Erroneus nickname"
# define ERR_ERRONEUSNICKNAME(client, nick)				": 432 " + client + " " + nick + " :Erroneus nickname"

//"<client> <nick> :Nickname is already in use"
# define ERR_NICKNAMEINUSE(client, nick)				": 433 " + client + " " + nick + " :Nickname is already in use"

//"<client> <nick> :Nickname collision KILL from <user>@<host>"
# define ERR_NICKCOLLISION(client, nick, user, host)	": 436 " + client + " " + nick + " :Nickname collision KILL from " + user + "@" + host

//"<client> <nick> <channel> :They aren't on that channel"
# define ERR_USERNOTINCHANNEL(client, nick, channel)	": 441 " + client + " " + nick + " " + channel + " :They aren't on that channel"

//"<client> <channel> :You're not on that channel"
# define ERR_NOTONCHANNEL(client, channel)				": 442 " + client + " " + channel + " :You're not on that channel"

//"<client> <nick> <channel> :is already on channel"
# define ERR_USERONCHANNEL(client, nick, channel)		": 443 " + client + " " + nick + " " + channel + " :is already on channel"

//"<client> :You have not registered"
# define ERR_NOTREGISTERED(client)						": 451 " + client + " :You have not registered"

//"<client> <command> :Not enough parameters"
# define ERR_NEEDMOREPARAMS(client, command)			": 461 " + client + " " + command + " :Not enough parameters"

//"<client> :You may not reregister"
# define ERR_ALREADYREGISTERED(client)					": 462 " + client + " :You may not reregister"

//"<client> :Password incorrect"
# define ERR_PASSWDMISMATCH(client)						": 464 " + client + " :Password incorrect"

//"<client> :You are banned from this server."
# define ERR_YOUREBANNEDCREEP(client)					": 465 " + client + " :You are banned from this server."

//"<client> <channel> :Cannot join channel (+l)"
# define ERR_CHANNELISFULL(client, channel)				": 471 " + client + " " + channel + " :Cannot join channel (+l)"

//"<client> <modechar> :is unknown mode char to me"
# define ERR_UNKNOWNMODE(client, modeChar)				": 472 " + client + " " + modeChar + " :is unknown mode char to me"

//"<client> <channel> :Cannot join channel (+i)"
# define ERR_INVITEONLYCHAN(client, channel)			": 473 " + client + " " + channel + " :Cannot join channel (+i)"

//"<client> <channel> :Cannot join channel (+b)"
# define ERR_BANNEDFROMCHAN(client, channel)			": 474 " + client + " " + channel + " :Cannot join channel (+b)"

//"<client> <channel> :Cannot join channel (+k)"
# define ERR_BADCHANNELKEY(client, channel)				": 475 " + client + " " + channel + " :Cannot join channel (+k)"

//"<client> <channel> :Bad Channel Mask"
# define ERR_BADCHANMASK(client, channel)				": 476 " + client + " " + channel + " :Bad Channel Mask"

//"<client> <channel> :You're not channel operator"
# define ERR_CHANOPRIVSNEEDED(client, channel)			": 482 " + client + " " + channel + " :You're not channel operator"

//"<client> <target chan> :Key is not well-formed"
# define ERR_INVALIDKEY(client, channel)				": 525 " + client + " " + channel + " :Key is not well-formed"

//"<client> <target chan/user> <mode char> <parameter> :<description>"
# define ERR_INVALIDMODEPARAM(client, channel, modeChar, parameter, notes)	": 696 " + client + " " + channel + " " + modeChar + " " + parameter + " :" + notes 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
