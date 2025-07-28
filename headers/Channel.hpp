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

/*
3. ChannelClass
    user_limit
    password (private Channels) - optional / can be NULL, _k für key    (private attribute)
    	KICK-      Eject a client from the channel
        INVITE-    Invite a client to a channel
        TOPIC-     Change or view the channel topic
        MODE-      Change the channels mode:
            · i: Set/remove Invite-only channel
            · t: Set/remove the restrictions of the TOPIC command to channel operators
            · k: Set/remove the channel key (password)
            · o: Give/take channel operator privilege
            · l: Set/remove the user limit to channel
*/
class Channel
{
	public:
		Channel(const std::string &name, const std::string &topic, const std::string &state, const std::string &mode);
		~Channel(void);

		std::string					getName(void) const;
		std::string					getTopic(void) const;
		void						setTopic(std::string topic);
		std::string					getState(void) const;
		void						setState(std::string state);
		std::string					getMode(void) const;
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

		std::string					_name; //Added By Stephan for compiling
		std::string					_topic;	//Added By Stephan for compiling
		std::string					_state;
		std::string					_mode;	// (e.g., +i, +t)
		std::vector<Client *>		_channelUsers;	//Added By Stephan for compiling
		std::vector<Client *>		_operators; // (ClientClass objs with operator status)
};

#endif
