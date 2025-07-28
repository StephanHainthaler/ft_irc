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
	if (port <= 0 || port > 65535)
		throw ServerException("Error. Invalid port number.");
	// AF_INET specifies IPv4 protocol family, SOCK_STREAM specifies TCP protocol
    _serverFd = socket(AF_INET, SOCK_STREAM, 0); // Create a TCP/IPv4 socket
    if (_serverFd == -1) 
	{
		throw ServerException("Error. Failed to create socket.");
	}
    
	_serverAddress.sin_family = AF_INET; // set the address family to IPv4 addresses
	_serverAddress.sin_addr.s_addr = INADDR_ANY; // server should accept connections from any IPv4 address, used when we don't want to bind our socket to any particular IP, to mak eit listen to all available IPs
	_serverAddress.sin_port = htons(port); // defines the port number the socket will use to communicate on server side (the value has to be in network byte order)

	std::cout << GRAY << "Server created with socket fd: " << _serverFd << ", port: " << port << ", password: " << password << DEFAULT << std::endl;
	
	// _clients and _channels remain empty at this point (?) - will get filled later

	_state = NOT_RUNNING; // Server state - 0: not running
}

Server::~Server(void)
{
	if (_serverFd != -1)
		close(_serverFd);

	// clear client map (WIP - needed?)
	if (!_clients.empty())
	{
		for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			delete it->second; // delete each Client object
		}
		_clients.clear();
	}
	/*
	// clear channel map (WIP - needed?)
	if (!_channels.empty())
	{
		for (std::vector<Client *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		{
			delete *it;
		}
		_channels.clear();
	}*/

	if (_ircClientFd != -1)
		close(_ircClientFd);
}

// Getters
std::string Server::getPassword(void) const
{
	return _password;
}

Channel *Server::getChannel(const std::string &channel_name) const
{
	for (std::vector<Channel *>::const_iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		if ((*it)->getName() == channel_name)
			return *it;
	}
	return NULL;
}

sockaddr_in Server::getServerAddress(void) const
{
	return _serverAddress;
}

int Server::getState(void) const
{
	return _state;
}

// Member functions - server actions
void Server::sendMessageToClient(int clientFD, std::string message)
{
	if (clientFD < 0)
	{
		std::cerr << RED << "Error. Invalid IRC client fd." << DEFAULT << std::endl;
		return;
	}
	
	long long totalSent = 0;
	long long msgLength = message.length();

	while (totalSent < msgLength) 
	{
		long long sent = send(clientFD, message.c_str() + totalSent, msgLength - totalSent, 0);
		if (sent == -1) 
			break;
		totalSent += sent;
	}
}

void Server::handleClientConnections(void)
{
	/*
	accept is like a reception desk that waits for clients to come in
	when they do, they get a new room (_ircClientFd) to communicate with the server
	accept blocks until a client connects, unless the server socket is set to non-blocking mode
	*/
	int clientFd = accept(_serverFd, NULL, NULL);
	if (clientFd < 0) // if a new client connected
	{
		std::cerr << RED << "Error. Failed to accept client connection." << DEFAULT << std::endl;
		return ;
	}
	std::cout << GRAY << "Client connected with fd: " << clientFd << DEFAULT << std::endl;

	/* pollfd  (useful: https://www.ibm.com/docs/en/i/7.4.0?topic=ssw_ibm_i_74/apis/poll.htm)
	is a structure used by poll() to monitor fds for readiness (for reading, writing, or errors)
	It contains:
	struct pollfd {
		int fd	// file descriptor to monitor, 
		int events;	//the events to monitor: POLLIN, POLLOUT
		int revents;	//what actually happened, which is set by poll()
	};
	*/
	pollfd pfd = {};
	pfd.fd = clientFd;
	pfd.events = POLLIN;
	pfd.revents = 0; // initially no events

	_pollfds.push_back(pfd);

	Client *newClient = new Client(clientFd, _port); // create a new Client object
	_clients.insert(std::make_pair(clientFd, newClient)); // add the new client to the map

	sendMessageToClient(clientFd, "Please enter the password to access the StePiaAn IRC server!\r\n"); // send welcome message to IRC client
}

/* https://modern.ircdocs.horse/
When reading messages from a stream, read the incoming data into a buffer. 
Only parse and process a message once you encounter the \r\n at the end of it. 
If you encounter an empty message, silently ignore it.
*/
void	Server::handleClientMessage(int clientFd)
{
	char 	buffer[MAX_MSG_LEN];

	bzero(buffer, MAX_MSG_LEN);
    int bytesReceived = recv(clientFd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT); // Flag for non-blocking
    if (bytesReceived > 0)
    {
		std::cout << "Client: " << buffer << std::endl; // first one is IRC client
		std::map<int, Client *>::iterator it = _clients.find(clientFd);
		handleInput(*it->second, buffer);
	}
}

void Server::handleClientDisconnections(int i)
{
	Client *client = _clients[_pollfds[i].fd];
	if (client)
	{
		client->setState(DISCONNECTED);
		client->disconnect();
		close(_pollfds[i].fd); // close hotel room (socket)
		_clients.erase(_pollfds[i].fd); // remove client from the map
		_pollfds.erase(_pollfds.begin() + i);
		std::cout << GRAY << "Client disconnected: " << _pollfds[i].fd << DEFAULT << std::endl;
	}
}

void Server::handleEvents(void)
{
	pollfd sfd;
	sfd.fd = _serverFd; // server socket
	sfd.events = POLLIN; // listen for incoming connections
	// POLLOUT = ready to write, POLLERR = error on the fd, POLLHUP = client closed socket
	sfd.revents = 0; // initially no events
	(pollfd)sfd;
	
	_pollfds.push_back(sfd); // add server socket to pollfds

	std::cout << GRAY << "Server is now running and waiting for events..." << DEFAULT << std::endl;

	while (_state == RUNNING) // while server is running
	{
		// Handle client input with poll
		int poll_count = poll(_pollfds.begin().base(), _pollfds.size(), -1);
		if (poll_count < 0) // poll returns the number of fds, which are ready to read, because of POLLIN
		{
			std::cerr << RED << "Error checking sockets for readiness. " << DEFAULT << std::endl;
			break;
		}
		else if (poll_count == 0) // no events occurred
		{
			std::cout << "No events occurred." << std::endl;
			continue; // retry polling
		}

		// else: if one or more sockets / fds are ready for reading

		// Check for events on each client socket
		for (int i = 0; i <= poll_count; ++i)
		{
			std::cout << "Checking pollfd[" << i << "] with fd: " << _pollfds[i].fd << " revents: " << _pollfds[i].revents << std::endl;

			// Case 1: there was no event with that fd
			if (_pollfds[i].revents == 0)
				continue; // skip to next fd

			// Case 2: the client disconnected
			/*
			Since revents is in binary, we can check for specific events using bitwise AND
			if they have the 1 in common, it will have true as a return
			*/
			if ((_pollfds[i].revents & POLLHUP) == POLLHUP) // if the client closed the connection
			{
				handleClientDisconnections(_pollfds[i].fd); // handle client disconnection
				break ;
			}

			// Case 3: there was an event with that fd
			if ((_pollfds[i].revents & POLLIN) == POLLIN) // if there's data to read
			{
				// for the server socket, data to read can be a new client connection
				if (_pollfds[i].fd == _serverFd) // if the server socket is
				{
					handleClientConnections(); // accept new client connection
					break; // skip to next fd
				}
				
				// for client sockets, data to read can be a message from another client
				handleClientMessage(_pollfds[i].fd);
			}
		}
	}
}

void Server::run(void)
{
	/* sockaddr_in vs sockaddr
	sockaddr_in is specifically for handling IPv4 addresses
	sockaddr is a generic structure that can be used for both IPv4 and IPv6 addresses
	and it is what the bind function expects
	*/
	if (bind(_serverFd, (struct sockaddr *)&_serverAddress, sizeof(_serverAddress)) == -1)
	{
		close(_serverFd);
		throw ServerException("Error. Failed to bind socket.");
	}
	
	// Marks a bound socket as "listening socket"
	// Server is on receiving end of data so needs to listen for incoming connections
	if (listen(_serverFd, SOMAXCONN) == -1) // SOMAXCONN is a constant that defines the maximum number of pending connections in the queue, 128 in most systems
	{
		close(_serverFd);
		throw ServerException("Error. Failed to listen on socket.");
	}
	_state = RUNNING; // Server state - 1: running
	
	// subject: "All I/O operations must be non-blocking"
	/*if (fcntl(_serverFd, F_SETFL, SOCK_NONBLOCK) == -1) // allow server to handle multiple clients at once
	{
		close(_serverFd);
		throw ServerException("Error. Failed to set socket to non-blocking mode.");
	}*/ // BUGFIX: implement this

	// Now the server is ready to handle incoming connections and client input
	handleEvents();
}

// Member functions - user triggered actions
/*
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
}*/

void toLowercase(const std::string& str)
{
	std::string result = str;
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
}

// For nickname changes within the same Client -- this will allow lower/upper case changes, for example: pia to Pia
bool Server::isNicknameAvailable(const std::string& nickname, const Client* excludeClient) const
{
    std::string lowerNick = nickname;
    toLowercase(lowerNick);
    
    for (std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        const Client* client = it->second;
        if (client && client != excludeClient)
        {
            std::string clientNick = client->getNickname();
            toLowercase(clientNick);
            if (clientNick == lowerNick)
                return (false);
        }
    }
    return (true);
}

// For new nicknames
bool Server::isNicknameAvailable(const std::string& nickname) const
{
    return (isNicknameAvailable(nickname, NULL));
}

void Server::handleNickCommand(Client* client, const std::string& newNickname)
{
	int clientFd = client->getSocketFD();
    
	// First check format using Client's validation
    if (client->isNickValid(newNickname) != 0)
    {
		// ERR_ERRONEUSNICKNAME (432) message, see https://modern.ircdocs.horse/#errnicknameinuse-433
		// std::string msg = client;
		std::string msg = newNickname;
		msg += " :Erroneus nickname";

		sendMessageToClient(clientFd, msg.c_str());
        return ;
    }
    
    // Then check uniqueness using Server's validation
    if (!isNicknameAvailable(newNickname, client))
    {
        // ERR_NICKNAMEINUSE (433) message, see https://modern.ircdocs.horse/#errnicknameinuse-433
		// std::string msg = client;
		std::string msg = newNickname;
		msg += " :Nickname is already in use";

		sendMessageToClient(clientFd, msg.c_str());
        return ;
    }
    
    // Nickname is valid and available
    client->setNick(newNickname);
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
