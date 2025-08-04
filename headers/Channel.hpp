/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shaintha <shaintha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 17:40:00 by shaintha          #+#    #+#             */
/*   Updated: 2025/07/09 17:40:00 by shaintha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include <vector>
# include <string>
# include <sstream>

# include "Client.hpp"

class Client;
class Server;
class Channel
{
	public:
		Channel(const std::string &name, const std::string &topic, const std::string &modes);
		~Channel(void);

		std::string					getName(void) const;

		std::string					getTopic(void) const;
		void						setTopic(std::string topic);

		std::string					getModes(void) const;
		bool						isValidChannelMode(char mode) const;
		void						setMode(char mode, bool enable);
		bool 						hasMode(char mode) const;
		std::string					getModeArguments(void) const;

		std::vector<Client *>		getChannelUsers(void) const;
		void						addUser(Client *client);
		void						removeUser(Client *client);
		Client						*getUser(const std::string &nickname) const;
		
		std::vector<Client *>		getOperators(void) const;
		void						addOperator(Client *client);
		void						removeOperator(Client *client);

		std::string					getChannelKey(void);
		void						setChannelKey(const std::string &key);

		unsigned int				getUserLimit(void) const;
		void						setUserLimit(unsigned int limit);

		std::string					getNamesOfChannelMembers(void) const;

	private:
		Channel(void);
		Channel(const Channel &other);
		Channel	&operator=(const Channel &other);

		std::string					_name;
		std::string					_topic;
		std::string					_modes;
		std::vector<Client *>		_channelUsers;

		std::string					_channelKey; // Channel key (password)
		std::vector<Client *>		_operators; // (ClientClass objs with operator status)
		unsigned int 				_userLimit; // User limit to channel

		/* Mode flags
		MODE <channel> +/-i  				| Invite-only
		MODE <channel> +/-t  				| Restrict topic changes to channel operators
		MODE <channel> +/-k <password> 		| Channel key (password)
		MODE <channel> +/-o <nickname> 		| Channel operator privilege
		MODE <channel> +/-l <limit> 		| User limit to channel
		
		CHANNEL_KEY
		https://modern.ircdocs.horse/#topic-message
		If this mode is set, its’ value is the key that is required. 
		Servers may validate the value (eg. to forbid spaces, as they make it harder to use the key in JOIN messages). 
		If the value is invalid, they SHOULD return ERR_INVALIDMODEPARAM

		if the channel has a key, can join only if the key is provided, meaning with
		JOIN #foo key
		*/
};