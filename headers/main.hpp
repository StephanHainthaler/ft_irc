/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shaintha <shaintha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 16:39:53 by shaintha          #+#    #+#             */
/*   Updated: 2025/07/01 16:39:53 by shaintha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <cstring> 

# define DEFAULT "\x1b[0m"
# define RED "\x1b[31m"
# define YELLOW "\x1b[33m"
# define GRAY "\x1b[90m"

enum ClientState
{
	CONNECTING,
	AUTHENTICATED,
	//UNREGISTERED,
	REGISTERED,
	DISCONNECTED
};