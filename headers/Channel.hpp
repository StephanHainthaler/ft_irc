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

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <vector>

# include "Client.hpp"

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
		int							setMode(char mode, bool enable);
		bool 						hasMode(char mode) const;

		std::vector<Client *>		getChannelUsers(void) const;
		std::vector<Client *>		getOperators(void) const;
		Client						*getUser(const std::string &nickname) const;

		void						addUser(Client *client);
		void						removeUser(Client *client);
		void						addOperator(Client *client);
		void						removeOperator(Client *client);


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
		MODE +-i  				| Invite-only
		MODE +-t  				| Restrict topic changes to channel operators
		MODE +-k <password> 	| Channel key (password)
		MODE +-o <nickname> 	| Channel operator privilege
		MODE +-l <limit> 		| User limit to channel
		
		CHANNEL_KEY
		https://modern.ircdocs.horse/#topic-message
		If this mode is set, its’ value is the key that is required. 
		Servers may validate the value (eg. to forbid spaces, as they make it harder to use the key in JOIN messages). 
		If the value is invalid, they SHOULD return ERR_INVALIDMODEPARAM
		*/
};

#endif
