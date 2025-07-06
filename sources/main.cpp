/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shaintha <shaintha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 16:30:15 by shaintha          #+#    #+#             */
/*   Updated: 2025/07/01 16:33:08 by shaintha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# define DEFAULT "\x1b[0m"
# define RED "\x1b[31m"
# define YELLOW "\x1b[33m"
# define GRAY "\x1b[90m"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

// ./ircserv <port> <password>
// g++ main.cpp -o ircserv -std=c++98
int	main() //int argc, char *argv[])
{
	argc = 3;
	argv[1] = "8080"; // Example port
	argv[2] = "password"; // Example password
	if (argc != 3)
		return (std::cerr << RED << "Error: incorrect number of arguments" << DEFAULT << std::endl, 1);
	
	int port = std::atoi(argv[1]);
	if (port <= 0 || port > 65535) // https://www.pico.net/kb/what-is-the-highest-tcp-port-number-allowed/
		return (std::cerr << RED << "Error: invalid port number" << DEFAULT << std::endl, 1);
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);  // Create a TCP/IPv4 socket
    if (sockfd == -1) 
    {
        std::cerr << "Error. Failed to create socket." << std::endl;
        return 1;
    }

	return 0;
}
