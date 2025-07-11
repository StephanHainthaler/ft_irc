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

#include <cstring>

enum ClientState
{
	CONNECTING,
	UNREGISTERED,
	REGISTERED,
	AUTHENTICATED,
	DISCONNECTED
};