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

class Channel
{
	public:
		~Channel(void);

		std::string					getName(void) const;
		std::vector<std::string>	getChannelUsers(void) const;
		std::string					getTopic(void) const;
		void						setTopic(std::string topic);

	private:
		Channel(void);
		Channel(const Channel &other);
		Channel	&operator=(const Channel &other);

		std::string					_name; //Added By Stephan for compiling
		std::vector<std::string>	_channelUsers;	//Added By Stephan for compiling
		std::string					_topic;	//Added By Stephan for compiling


};

#endif
