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

class Channel
{
	public:
		~Channel(void);

	private:
		Channel(void);
		Channel(const Channel &other);
		Channel	&operator=(const Channel &other);


};

#endif
