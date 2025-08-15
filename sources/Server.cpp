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

Server::Server(const std::string &portString, const std::string &password): _name("localhost"), _port(std::atoi(portString.c_str())), _password(password)
{
	if (isPositiveNumber(portString) == false || _port > 65535)
		throw ServerException("Error. Invalid port number.");

	// AF_INET specifies IPv4 protocol family, SOCK_STREAM specifies TCP protocol
    _serverFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); // Create a TCP/IPv4 socket
    if (_serverFd == -1) 
		throw ServerException("Error. Failed to create socket.");
	
	/* sockaddr_in "serverAddress" to specify its own IP address and port to bind to
	client takes this sockaddr_in "serverAddress" to specify the server's IP address and port to connect to */
	_serverAddress.sin_family = AF_INET;			// set the address family to IPv4 addresses
	_serverAddress.sin_addr.s_addr = INADDR_ANY;	// server should accept connections from any IPv4 address, used when we don't want to bind our socket to any particular IP, to mak eit listen to all available IPs
	_serverAddress.sin_port = htons(_port);			// defines the port number the socket will use to communicate on server side (the value has to be in network byte order)
	std::cout << YELLOW << "[SERVER] created with socket fd: " << _serverFd << ", port: " << _port << ", password: " << password << DEFAULT << std::endl;
}

Server::~Server(void)
{
	std::cout << YELLOW << "[SERVER] is shutting down." << DEFAULT << std::endl;
	
	if (_serverFd != -1)
		close(_serverFd);
	if (!_clients.empty())
	{
		for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			close(it->first);
			delete it->second;
		}
		_clients.clear();
	}
	_outgoingMessages.clear();
	_incomingMessages.clear();
	if (!_channels.empty())
	{
		for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
			delete *it;

		_channels.clear();
	}
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

void	Server::setName(std::string name)
{
	_name = name;
}

std::string	Server::getName(void) const
{
	return (_name);
}

std::string	Server::getPassword(void) const
{
	return (_password);
}

sockaddr_in	Server::getServerAddress(void) const
{
	return (_serverAddress);
}

Client	*Server::getClient(std::string nickname) const
{
	for (std::map<int, Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
			return it->second;
	}
	return NULL;
}

Channel	*Server::getChannel(const std::string &channel_name) const
{
	for (std::vector<Channel *>::const_iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		if ((*it)->getName() == channel_name)
			return *it;
	}
	return NULL;
}

void	Server::run(void)
{
	// to manipulate options at the sockets API level, "level" is specified as SOL_SOCKET
	int yes = 1;
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
        throw ServerException("Error. Failed to configure socket.");
	if (bind(_serverFd, (struct sockaddr *)&_serverAddress, sizeof(_serverAddress)) == -1)
	{
		close(_serverFd);
		throw ServerException("Error. Failed to bind socket.");
	}
	if (listen(_serverFd, MAXCONN) == -1) // MAXCONN - max possible number of pending connection requests for the socket
	{
		close(_serverFd);
		throw ServerException("Error. Failed to listen on socket.");
	}
	setupSignals();
	while (g_shutdown == 0)
		handleEvents();
}

void	Server::setupSignals()
{
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGQUIT, signalHandler);
	signal(SIGPIPE, SIG_IGN);
}

void	signalHandler(int sig)
{
    if (sig == SIGINT || sig == SIGTERM || sig == SIGQUIT)
	{
        g_shutdown = 1;
    }
}

void	Server::handleEvents(void)
{
	pollfd	sfd;

	sfd.fd = _serverFd;
	sfd.events = POLLIN; // POLLIN = ready to read, POLLOUT = ready to write but server is a listening socket, so we only care about POLLIN
	sfd.revents = 0; // initially no events
	_pollfds.push_back(sfd);
	std::cout << YELLOW << "[SERVER] is now running and waiting for events..." << DEFAULT << std::endl;
	while (!g_shutdown) // while server is running and no shutdown signal
	{
		int poll_count = poll(_pollfds.begin().base(), _pollfds.size(), -1);
		if (poll_count < 0) // number of fds, which are ready to read, because of POLLIN
			std::cout << GRAY << "No (more) poll events." << DEFAULT << std::endl;
		else if (poll_count == 0) // no events occurred
		{
			std::cout << GRAY << "No events occurred." << DEFAULT << std::endl;
			continue; // retry polling
		}
		// else: if one or more sockets / fds are ready for reading (or writing)
		for (size_t i = 0; i < _pollfds.size(); ++i)
		{
			// Case 1: there was no event with that fd
			if (_pollfds[i].revents == 0)
				continue;

			// Case 2: the client disconnected
			if ((_pollfds[i].revents & POLLHUP) == POLLHUP) // if the client closed the connection
			{
				handleClientDisconnections(i);
				break;
			}
			// Case 3: there was an event with that fd
			if ((_pollfds[i].revents & POLLIN) == POLLIN) // if there's data to read
			{
				// for the server socket, data to read can be a new client connection
				if (_pollfds[i].fd == _serverFd)
				{
					handleClientConnections();
					break;
				}
				// for client sockets, data to read can be a message from another client
				handleClientMessage(_pollfds[i].fd);
			}
			// Case 4: there is data to send to the client => check if reverts caught POLLOUT
			if ((_pollfds[i].revents & POLLOUT) == POLLOUT)
				handleSendingToClient(i);
		}
	}
}

void	Server::handleClientConnections(void)
{
	/* accept is like a reception desk that waits for clients to come in
	when they do, they get a new room (_clientFd) to communicate with the server
	accept blocks until a client connects, unless the server socket is set to non-blocking mode */
	int clientFd = accept(_serverFd, NULL, NULL);
	if (clientFd < 0) 
	{
		std::cerr << RED << "Error. Failed to accept client connection." << DEFAULT << std::endl;
		return ;
	}
	std::cout << GRAY << "Client connected with fd: " << clientFd << DEFAULT << std::endl;
	/* pollfd = structure used by poll() to monitor fds for readiness (for reading, writing, or errors)
	It contains:
	struct pollfd {
		int fd	// file descriptor to monitor, 
		int events;	//the events to monitor: POLLIN, POLLOUT
		int revents;	//what actually happened, which is set by poll()
	}; */
	pollfd pfd = {};
	pfd.fd = clientFd;
	pfd.events = POLLIN | POLLHUP;
	pfd.revents = 0; // initially no events
	_pollfds.push_back(pfd);
	Client *newClient = new Client(clientFd); // create and register a new client
	_clients.insert(std::make_pair(clientFd, newClient));
	_outgoingMessages.insert(std::make_pair(clientFd, ""));
	_incomingMessages.insert(std::make_pair(clientFd, ""));
	sendMessageToClient(clientFd, "Please enter the password to access the StePiaAn IRC server!"); // send welcome message to IRC client
}

void	Server::handleClientDisconnections(int i)
{
	Client *client = _clients[_pollfds[i].fd];

	if (client)
	{
		client->disconnect(); // also closes the "hotel room" (socket)
		std::cout << GRAY << "Client disconnected with fd: " << _pollfds[i].fd << DEFAULT << std::endl;
		close(_pollfds[i].fd);
		_clients.erase(_pollfds[i].fd);
		_pollfds.erase(_pollfds.begin() + i);
		_outgoingMessages.erase(_pollfds[i].fd);
		_incomingMessages.erase(_pollfds[i].fd);
		delete client;
	}
}

void	Server::handleSendingToClient(int i)
{
	std::string &buffer = _outgoingMessages[_pollfds[i].fd];

	if (!buffer.empty())
	{
		ssize_t sent = send(_pollfds[i].fd, buffer.c_str(), buffer.size(), MSG_DONTWAIT); // makes the call non-blocking

		if (sent > 0)
			buffer.erase(0, sent); // remove sent bytes
	}

	if (buffer.empty())
		_pollfds[i].events = POLLIN | POLLHUP; // stop checking for POLLOUT
}

/* https://modern.ircdocs.horse/
When reading messages from a stream, read the incoming data into a buffer. 
Only parse and process a message once you encounter the \r\n at the end of it. */
void	Server::handleClientMessage(int clientFd)
{
	char	buffer[MAX_MSG_LEN];
	size_t 	pos;

	bzero(buffer, sizeof(buffer));
	// call recv once per poll event
	int bytesReceived = recv(clientFd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT); // Flag makes the call non-blocking
    if (bytesReceived <= 0)
		return ;
	buffer[bytesReceived] = '\0';
	_incomingMessages[clientFd] += std::string(buffer); // add data to the client's incoming buffer	
	/* in nc, ctrl+d is used to signal EOF, in which case the input is sent
	BUT and IRC message is a single line delimited by \r\n */
	while((pos = _incomingMessages[clientFd].find("\r\n")) != std::string::npos)
	{
		std::string message = _incomingMessages[clientFd].substr(0, pos);
		_incomingMessages[clientFd].erase(0, pos + 2); // remove the processed message from the buffer (incl \r\n)
		if (!message.empty())
		{
			// std::cout << GRAY << "Client (fd = " << clientFd << "): " << message << DEFAULT << std::endl; // first one is IRC client
			Client *client = _clients[clientFd];
			handleInput(*client, message);
		}
	}
}

void	Server::sendMessageToClient(int clientFD, std::string message)
{
    if (clientFD < 0)
    {
        std::cerr << RED << "Error. Invalid client fd." << DEFAULT << std::endl;
        return;
    }
    // Add message to the outgoing buffer for this client
    _outgoingMessages[clientFD] += message + "\r\n";

	// start checking for the POLLOUT event too
	for (size_t i = 0; i < _pollfds.size(); ++i)
	{
		if (_pollfds[i].fd == clientFD)
		{
			_pollfds[i].events |= POLLOUT; 
			break;
		}
	}
}

void	Server::sendMessageToChannel(Channel* channel, const std::string& message)
{
	std::vector<Client*> usersInChannel = channel->getOperators();

	for (size_t i = 0; i < usersInChannel.size(); i++)
		sendMessageToClient(usersInChannel[i]->getSocketFD(), message);
	usersInChannel = channel->getChannelUsers();
	for (size_t i = 0; i < usersInChannel.size(); i++)
		sendMessageToClient(usersInChannel[i]->getSocketFD(), message);
}

void	Server::sendMessageToChannel(Client* excludedSender, Channel* channel, const std::string& message)
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

void	Server::createChannel(std::string &newChannelName, Client &founder)
{
	Channel *newChannel = new Channel(newChannelName, founder);
	_channels.push_back(newChannel);

	sendMessageToClient(founder.getSocketFD(), MSG_JOIN(founder.getNickname(), newChannelName));
	sendMessageToClient(founder.getSocketFD(), RPL_NOTOPIC(getName(), founder.getNickname(), newChannelName));
	sendMessageToClient(founder.getSocketFD(), RPL_NAMREPLY(getName(), founder.getNickname(), "=", newChannelName, newChannel->getNamesOfChannelMembers()));
	sendMessageToClient(founder.getSocketFD(), RPL_ENDOFNAMES(getName(), founder.getNickname(), newChannelName));
}

void	Server::removeChannel(Channel *channel)
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

Server::ServerException::ServerException(const std::string &message): _message(message)
{
}

Server::ServerException::~ServerException() throw() 
{
}

const char*	Server::ServerException::what() const throw()
{
	return (_message.c_str());
}
