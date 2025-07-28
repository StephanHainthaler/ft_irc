/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shaintha <shaintha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 17:40:33 by shaintha          #+#    #+#             */
/*   Updated: 2025/07/09 17:40:33 by shaintha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Channel.hpp"

std::string Channel::getName(void) const
{
	return _name;
}

std::vector<std::string> Channel::getChannelUsers(void) const
{
	return _channelUsers;
}

std::string	Channel::getTopic(void) const
{
	return _topic;
}

void	Channel::setTopic(std::string topic)
{
	_topic = topic;
}
