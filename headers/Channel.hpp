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

# include "Server.hpp"

class Client;

class Channel
{
	public:
		Channel(const std::string &name, Client &creator);
		~Channel(void);

		// Getters & Setters
		void					setName(const std::string &name);
		std::string				getName(void) const;
		int						setOperator(std::string &nickname, bool enable, std::string &comment);
		std::vector<Client *>	getOperators(void) const;
		std::vector<Client *>	getChannelUsers(void) const;
		std::string				getNamesOfChannelMembers(void) const;
		Client					*getUser(const std::string &nickname) const;
		int						setMode(char mode, std::string modearg, bool enable, std::string &comment, size_t &cmdNumber);
		std::string				getModes(void) const;
		std::string				getModeArguments(void) const;
		void					setTopic(std::string topic);
		std::string				getTopic(void) const;
		void					setChannelKey(const std::string &key);
		std::string				getChannelKey(void);
		void					setUserLimit(const size_t &limit);
		size_t					getUserLimit(void) const;

		bool					isOperator(Client *client) const;
		bool					isValidChannelMode(char mode) const;
		bool 					hasMode(char mode) const;
		
		void					addUser(Client *client);
		void					removeUser(Client *client);

	private:
		std::string				_name;
		std::vector<Client *>	_operators;
		std::vector<Client *>	_channelUsers;
		std::string				_modes;
		std::string				_topic;
		std::string				_channelKey;
		size_t 					_userLimit;
};

#endif
