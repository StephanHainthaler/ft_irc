/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shaintha <shaintha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/16 09:22:12 by juitz             #+#    #+#             */
/*   Updated: 2025/08/15 10:50:47 by shaintha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Server.hpp"

bool	isPositiveNumber(std::string string)
{
	const char	*cString = string.c_str();
	size_t	i = 0, end = std::strlen(cString);

	if (cString[i] == '\0' || cString == NULL)
		return (false);
	while (isspace(cString[i]) == true)
		i++;
	if (cString[i] == '-')
		return (false);
	if (cString[i] == '+')
		i++;
	if (!(cString[i] >= '0' && cString[i] <= '9'))
		return (false);
	for ( ; (cString[i] >= '0' && cString[i] <= '9'); i++)
	{
	}
	if (i != end)
		return (false);
	return (true);
}

std::string toLowercase(const std::string &string)
{
	std::string result = string;
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
	return (result);
}

void    Server::parseStringToVector(std::string string, std::vector<std::string> *vector, const char *delimiters)
{
	for (char* token = std::strtok((char *)string.c_str(), delimiters); token; token = std::strtok(NULL, delimiters))
		vector->push_back(token);
}

size_t  Server::getStringPosition(std::string &string, size_t numberOfArguments)
{
	size_t pos = 0;

	for (size_t i = 0; i < numberOfArguments; i++)
	{
		while (std::isspace(string[pos]) == true)
			string[pos++];
		if (i + 1 == numberOfArguments)
			return (pos);
		while (std::isspace(string[pos]) == false)
			string[pos++];
	}
	return (pos);
}

bool    Server::isNicknameAvailable(const std::string &nickname, const Client *targetClient) const
{
    std::string targetNickNew = toLowercase(nickname);

    for (std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
		const Client* client = it->second;
		std::string clientNick = toLowercase(client->getNickname());

		// if the nickname or any case-variation of it is already taken by another client
        if (client && client != targetClient && clientNick == targetNickNew)
            return (false);		
    }
    return (true);
}
