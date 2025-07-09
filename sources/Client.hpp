/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juitz <juitz@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 13:53:54 by juitz             #+#    #+#             */
/*   Updated: 2025/07/09 17:31:18 by juitz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <cstddef>
#include <iostream>
#include <string>

class Client
{
	private:
		size_t		_port;
		size_t		_clientSocketFd;
		std::string	_IP;
		std::string _userName;
		std::string _nickName;
		std::string _realName;
	
	public:
		int	isNickValid(const std::string& nickName);

		//USER_function
		//NICK_function
		//JOIN_function
		
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