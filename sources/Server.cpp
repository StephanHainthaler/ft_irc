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

volatile sig_atomic_t g_shutdown = 0;

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
    _serverFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); // Create a TCP/IPv4 socket
    if (_serverFd == -1) 
		throw ServerException("Error. Failed to create socket.");

    
	_serverAddress.sin_family = AF_INET; // set the address family to IPv4 addresses
	_serverAddress.sin_addr.s_addr = INADDR_ANY; // server should accept connections from any IPv4 address, used when we don't want to bind our socket to any particular IP, to mak eit listen to all available IPs
	_serverAddress.sin_port = htons(port); // defines the port number the socket will use to communicate on server side (the value has to be in network byte order)

	std::cout << GRAY << "Server created with socket fd: " << _serverFd << ", port: " << port << ", password: " << password << DEFAULT << std::endl;
	
	// _clients and _channels remain empty at this point (?) - will get filled later

	_state = NOT_RUNNING; // Server state - 0: not running


	//ADDED BY STEPHAN
	_name = "localhost";

}

Server::~Server(void)
{
	std::cout << YELLOW << "\n[SERVER] Starting graceful shutdown..." << DEFAULT << std::endl;
	
	// Notify all connected clients about server shutdown
	std::string shutdownMsg = "ERROR: Server shutting down";
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		sendMessageToClient(it->first, shutdownMsg);
		std::cout << GRAY << "Notified client " << it->first << " about shutdown" << DEFAULT << std::endl;
	}
	
	// Give clients a moment to receive the message
	usleep(100000); // 100ms
	
	// Close the server socket
	if (_serverFd != -1)
		close(_serverFd);

	// Close all client connections
	if (!_clients.empty())
	{
		for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			close(it->first); // close the socket
			delete it->second;
		}
		_clients.clear();
	}

	// Clear outgoing messages
	_outgoingMessages.clear();

	// Clear channels
	if (!_channels.empty())
	{
		for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
			delete *it;

		_channels.clear();
	}
	
	// close all in pollfds
	if (!_pollfds.empty())
	{
		for (size_t i = 0; i < _pollfds.size(); ++i)
		{
			if (_pollfds[i].fd != -1)
				close(_pollfds[i].fd);
		}
		_pollfds.clear();
	}
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

Client *Server::getClient(std::string nickname) const
{
	for (std::map<int, Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
			return it->second;
	}
	return NULL;
}

// Member functions - server actions
void Server::sendMessageToClient(int clientFD, std::string message)
{
    if (clientFD < 0)
    {
        std::cerr << RED << "Error. Invalid client fd." << DEFAULT << std::endl;
        return;
    }

    // Add message to the outgoing buffer for this client
    _outgoingMessages[clientFD] += message + "\r\n";

	// set pollout
	for (size_t i = 0; i < _pollfds.size(); ++i)
	{
		if (_pollfds[i].fd == clientFD)
		{
			_pollfds[i].events |= POLLOUT; // start checking for the POLLOUT event too
			break;
		}
	}
}

void Server::handleSendingToClient(int i)
{
	std::string &buffer = _outgoingMessages[_pollfds[i].fd];

	if (!buffer.empty())
	{
		ssize_t sent = send(_pollfds[i].fd, buffer.c_str(), buffer.size(), MSG_DONTWAIT); // makes the call non-blocking

		if (sent > 0)
			buffer.erase(0, sent); // remove sent bytes
	}

	if (buffer.empty())
		_pollfds[i].events &= ~POLLOUT; // remove POLLOUT event
}


void Server::sendMessageToChannel(Channel* channel, const std::string& message)
{
	std::vector<Client*> usersInChannel = channel->getOperators();

	for (size_t i = 0; i < usersInChannel.size(); i++)
		sendMessageToClient(usersInChannel[i]->getSocketFD(), message);
	usersInChannel = channel->getChannelUsers();
	for (size_t i = 0; i < usersInChannel.size(); i++)
		sendMessageToClient(usersInChannel[i]->getSocketFD(), message);
}

void Server::sendMessageToChannel(Client* excludedSender, Channel* channel, const std::string& message)
{
	std::vector<Client*> usersInChannel = channel->getOperators();

	for (size_t i = 0; i < usersInChannel.size(); i++)
	{
		Client* targetClient = usersInChannel[i];
		
		if (targetClient != excludedSender)
			sendMessageToClient(targetClient->getSocketFD(), message);
	}
	usersInChannel = channel->getChannelUsers();
	for (size_t i = 0; i < usersInChannel.size(); i++)
	{
		Client* targetClient = usersInChannel[i];
		
		if (targetClient != excludedSender)
			sendMessageToClient(targetClient->getSocketFD(), message);
	}
}

void Server::handleClientConnections(void)
{
	/*
	accept is like a reception desk that waits for clients to come in
	when they do, they get a new room (_clientFd) to communicate with the server
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
	pfd.events = POLLIN | POLLHUP;
	pfd.revents = 0; // initially no events

	_pollfds.push_back(pfd);

	Client *newClient = new Client(clientFd, _port); // create a new Client object
	_clients.insert(std::make_pair(clientFd, newClient)); // add the new client to the map
	_outgoingMessages.insert(std::make_pair(clientFd, "")); // initialize the outgoing buffer for the new client

	sendMessageToClient(clientFd, "Please enter the password to access the StePiaAn IRC server!"); // send welcome message to IRC client
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
    int bytesReceived = recv(clientFd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT); // Flag which makes the call non-blocking
    if (bytesReceived > 0)
    {
		std::cout << GRAY << "Client (fd = " << clientFd << "): " << buffer << DEFAULT << std::endl; // first one is IRC client
		std::map<int, Client *>::iterator it = _clients.find(clientFd);
		Client	*client = it->second;
		handleInput(*client, buffer);
	}
}

void Server::handleClientDisconnections(int i)
{
	Client *client = _clients[_pollfds[i].fd];
	if (client)
	{
		client->disconnect(); // also closes the "hotel room" (socket)
		std::cout << YELLOW << "Client disconnected: " << _pollfds[i].fd << DEFAULT << std::endl;
		_clients.erase(_pollfds[i].fd); // remove client from the map
		_pollfds.erase(_pollfds.begin() + i); // remove client from pollfds
		_outgoingMessages.erase(_pollfds[i].fd); // remove client's outgoing buffer
		delete client; // delete Client object
	}
}

void Server::handleEvents(void)
{
	pollfd sfd;
	sfd.fd = _serverFd; // server socket
	sfd.events = POLLIN; // POLLIN = ready to read, POLLOUT = ready to write but server is a listening socket, so we only care about POLLIN
	sfd.revents = 0; // initially no events
	
	_pollfds.push_back(sfd); // add server socket to pollfds

	std::cout << GRAY << "Server is now running and waiting for events..." << DEFAULT << std::endl;

	while (_state == RUNNING && !g_shutdown) // while server is running and no shutdown signal
	{
		// Handle client input with poll
		int poll_count = poll(_pollfds.begin().base(), _pollfds.size(), -1);
		if (poll_count < 0) // poll returns the number of fds, which are ready to read, because of POLLIN
			throw ServerException("Error. Could not check sockets for events.");
		
		else if (poll_count == 0) // no events occurred
		{
			std::cout << GRAY << "No events occurred." << DEFAULT << std::endl;
			continue; // retry polling
		}

		// else: if one or more sockets / fds are ready for reading or writing

		// Check for events on each client socket
		for (size_t i = 0; i < _pollfds.size(); ++i)
		{
			//std::cout << GRAY << "Checking pollfd[" << i << "] with fd: " << _pollfds[i].fd << " revents: " << _pollfds[i].revents <<  DEFAULT << std::endl;

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
				handleClientDisconnections(i);
				break ; // skip to next fd
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

			// Case 4: there is data to send to the client => check if reverts caught POLLOUT
			if ((_pollfds[i].revents & POLLOUT) == POLLOUT)
			{
				handleSendingToClient(i);
			}
		}
	}
}

void Server::run(void)
{
	/* linux.die.net/man/2/setsockopt AND https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-setsockopt
	to manipulate options at the sockets API level, "level" is specified as SOL_SOCKET
	SO_REUSEADDR 	BOOL 	Allows the socket to be bound to an address that is already in use- see bind.
	*/
	int yes = 1; // value to set the option to
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) // set socket option to allow address reuse
        throw ServerException("Error. Failed to configure socket.");
		
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
	
	// subject: "All I/O operations must be non-blocking" - set non-blocking at socket creation
	// used SOCK_NONBLOCK flag in socket creation, which is equivalent to fcntl usage
	/*if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) == -1) // allow server to handle multiple clients at once
	{
		close(_serverFd);
		throw ServerException("Error. Failed to set socket to non-blocking mode.");
	}*/

	// Now the server is ready to handle incoming connections and client input
	setupSignals();
	while (g_shutdown == 0)
		handleEvents();
	throw ServerException("Server shutdown complete.");
}

// Member functions - user triggered actions
// void Server::addChannel(Channel *channel, Client &founder)
// {
// 	if (channel == NULL)
// 		return;
// 	for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
// 		if (*it == channel)
// 			return; 
// 	_channels.push_back(channel);
// }

void Server::removeChannel(Channel *channel)
{
	if (channel == NULL)
		return;

	for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		if (*it == channel)
		{
			_channels.erase(it);
			delete channel;
			break;
		}
	}
}

// ADDED BY STEPHAN

void	Server::createChannel(std::string &newChannelName, Client &founder)
{
	Channel *newChannel = new Channel(newChannelName, founder);
	_channels.push_back(newChannel);

	sendMessageToClient(founder.getSocketFD(), MSG_JOIN(founder.getNickname(), newChannelName));
	sendMessageToClient(founder.getSocketFD(), MSG_MODE(getName(), newChannelName, "+o", founder.getNickname()));
	sendMessageToClient(founder.getSocketFD(), RPL_NOTOPIC(getName(), founder.getNickname(), newChannelName));
	sendMessageToClient(founder.getSocketFD(), RPL_NAMREPLY(getName(), founder.getNickname(), "=", newChannelName, newChannel->getNamesOfChannelMembers()));
	sendMessageToClient(founder.getSocketFD(), RPL_ENDOFNAMES(getName(), founder.getNickname(), newChannelName));
}





std::string toLowercase(const std::string& str)
{
	std::string result = str;
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
	return (result);
}

void signalHandler(int sig)
{
    if (sig == SIGINT || sig == SIGTERM)
	{
        g_shutdown = 1;
    }
}

void Server::setupSignals()
{
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGPIPE, SIG_IGN);
}

// For nickname changes within the same Client -- this will allow lower/upper case changes, for example: pia to Pia
bool Server::isNicknameAvailable(const std::string& nickname, const Client* targetClient) const
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


//ADDED BY STEPHAN
void	Server::setName(std::string name)
{
	_name = name;
}

std::string	Server::getName(void) const
{
	return (_name);
}
