/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juitz <juitz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 13:53:54 by juitz             #+#    #+#             */
/*   Updated: 2025/07/09 15:27:11 by juitz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <string>

class Client
{
	private:
		int			_fd;
		std::string	_IP;
		std::string _nickName;
		std::string _realName;
	
	public:
		int	isNickValid(const std::string& nickName);
		
		
		Client();
		~Client();
		Client(const Client &other);
		Client& operator=(const Client &copy);

		class NickNameTooLong : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
		class NickNameInvalid: public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
};