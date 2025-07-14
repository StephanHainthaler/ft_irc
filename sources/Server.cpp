/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 15:52:56 by shaintha          #+#    #+#             */
/*   Updated: 2025/07/09 15:52:56 by shaintha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Server.hpp"

/* sockaddr_in / sockaddr_in6 
is a structure specifically for handling IPv4 addresses
subject: "communication between client and server has to be done via TCP/IP (v4 or v6)"

the IP address is specified by the computer I am running the server code on,
and the port number is specified by the user (as a command line argument)
*/
Server::Server(const unsigned int &port, const std::string &password): _port(port), _password(password)
{
	// AF_INET specifies IPv4 protocol family, SOCK_STREAM specifies TCP protocol
    _socket_fd = socket(AF_INET, SOCK_STREAM, 0); // Create a TCP/IPv4 socket
    if (_socket_fd == -1) 
	{
		throw ServerException("Error. Failed to create socket.");
	}

	// subject: "All I/O operations must be non-blocking"
	if (fcntl(_socket_fd, F_SETFL, SOCK_NONBLOCK) == -1) // allow server to handle multiple clients at once
	{
		close(_socket_fd);
		throw ServerException("Error. Failed to set socket to non-blocking mode.");
	}
    
	_serverAddress.sin_family = AF_INET; // set the address family to IPv4 addresses
	_serverAddress.sin_addr.s_addr = INADDR_ANY; // server should accept connections from any IPv4 address, used when we don't want to bind our socket to any particular IP, to mak eit listen to all available IPs
	_serverAddress.sin_port = htons(port); // defines the port number the socket will use to communicate on server side (the value has to be in network byte order)

	// _clients and _channels remain empty at this point (?) - will get filled later

	_state = 0; // Server state - 0: not running
}

Server::~Server(void)
{
	if (_socket_fd != -1)
		close(_socket_fd);

	// clear client vector (BUGFIX? - needed?)
	if (!_clients.empty())
	{
		for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			delete *it;
		}
		_clients.clear();
	}

	// clear channel vector (BUGFIX? - needed?)
	if (!_channels.empty())
	{
		for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		{
			delete *it;
		}
		_channels.clear();
	}
}

void Server::run()
{
	/* sockaddr_in vs sockaddr
	sockaddr_in is specifically for handling IPv4 addresses
	sockaddr is a generic structure that can be used for both IPv4 and IPv6 addresses
	and it is what the bind function expects
	*/
	if (bind(_socket_fd, (struct sockaddr *)&_serverAddress, sizeof(_serverAddress)) == -1)
	{
		close(_socket_fd);
		throw ServerException("Error. Failed to bind socket.");
	}
	
	// Marks a bound socket as "listening socket"
	// Server is on receiving end of data so needs to listen for incoming connections
	if (listen(_socket_fd, SOMAXCONN) == -1)
	{
		close(_socket_fd);
		throw ServerException("Error. Failed to listen on socket.");
	}

	std::cout << "Server started on port " << _port << std::endl;

	_state = 1; // Server state - 1: running
}

// Getters
std::string Server::getPassword(void) const
{
	return _password;
}

/*Channel *Server::getChannel(const std::string &channel_name) const
{
	for (std::vector<Channel *>::const_iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		if ((*it)->get_name() == channel_name)
			return *it;
	}
	return NULL;
}*/

sockaddr_in Server::getServerAddress(void) const
{
	return _serverAddress;
}

int Server::getState(void) const
{
	return _state;
}

// Member functions - server actions
void Server::acceptClientConnection(Client *client)
{
	if (client == NULL || _state != 1) // Server must be running to accept connections
		return;

	// password check - subject: "The connection pw will be needed by any IRC client that tries to connect to your server"
	if (client->get_password() != _password)
	{
		std::cerr << RED << "Error. Wrong client password." << DEFAULT << std::endl;
		return;
	}

	int client_fd = accept(_socket_fd, NULL, NULL);
	if (client_fd == -1) 
	{
		std::cerr << RED << "Error. Failed to accept connection." << DEFAULT << std::endl;
		return;
	}
	addClient(client);
}	

void Server::handleClientMessage(int client_fd)
{
	if (client_fd < 0)
	{
		std::cerr << RED << "Error. Invalid client fd." << DEFAULT << std::endl;
		return;
	}

	// in progress
}

// Member functions - user triggered actions
void Server::addClient(Client *client)
{
	if (client == NULL)
		return;
	
	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (*it == client)
			return; 
	}
	_clients.push_back(client);
}

void Server::removeClient(Client *client)
{
	if (client == NULL)
		return;

	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (*it == client)
		{
			_clients.erase(it);
			break;
		}
	}
}

void Server::addChannel(Channel *channel)
{
	if (channel == NULL)
		return;

	for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		if (*it == channel)
			return; 
	}
	_channels.push_back(channel);
}

void Server::removeChannel(Channel *channel)
{
	if (channel == NULL)
		return;

	for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		if (*it == channel)
		{
			_channels.erase(it);
			break;
		}
	}
}

// Exception
Server::ServerException::ServerException(const std::string &message): _message(message)
{
}

const char* Server::ServerException::what() const throw()
{
	return _message.c_str();
}

Server::ServerException::~ServerException() throw() 
{
}