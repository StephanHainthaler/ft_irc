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
	if (atoi(port.c_str()) <= 0 || atoi(port.c_str()) > 65535)
		throw ServerException("Error. Invalid port number.");
	// AF_INET specifies IPv4 protocol family, SOCK_STREAM specifies TCP protocol
    _server_fd = socket(AF_INET, SOCK_STREAM, 0); // Create a TCP/IPv4 socket
    if (_server_fd == -1) 
	{
		throw ServerException("Error. Failed to create socket.");
	}
    
	_serverAddress.sin_family = AF_INET; // set the address family to IPv4 addresses
	_serverAddress.sin_addr.s_addr = INADDR_ANY; // server should accept connections from any IPv4 address, used when we don't want to bind our socket to any particular IP, to mak eit listen to all available IPs
	_serverAddress.sin_port = htons(port); // defines the port number the socket will use to communicate on server side (the value has to be in network byte order)

	std::cout << GRAY << "Server created with socket fd: " << _server_fd << ", port: " << port << ", password: " << password << DEFAULT << std::endl;
	
	// _clients and _channels remain empty at this point (?) - will get filled later

	_state = NOT_RUNNING; // Server state - 0: not running
}

Server::~Server(void)
{
	if (_server_fd != -1)
		close(_server_fd);

	// clear client vector (WIP - needed?)
	if (!_clients.empty())
	{
		for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			delete *it;
		}
		_clients.clear();
	}
	/*
	// clear channel vector (WIP - needed?)
	if (!_channels.empty())
	{
		for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		{
			delete *it;
		}
		_channels.clear();
	}*/

	if (_IRC_client_fd != -1)
		close(_IRC_client_fd);
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
void Server::sendMessageToIRCClient(const char* msg)
{
	if (_IRC_client_fd < 0)
	{
		std::cerr << RED << "Error. Invalid IRC client fd." << DEFAULT << std::endl;
		return;
	}
	
	long long total_sent = 0;
	long long msg_len = strlen(msg);

	while (total_sent < msg_len) 
	{
		long long sent = send(_IRC_client_fd, msg + total_sent, msg_len - total_sent, 0);
		if (sent == -1) 
			break;
		total_sent += sent;
	}
}

void Server::handleClientConnections()
{
	sockaddr_in clientAddress = {}; // need to catch it with accept
	socklen_t clientAddressLen = sizeof(clientAddress);
	
	// Handle incoming connections
	int client_fd = accept(_server_fd, (sockaddr *)&clientAddress, &clientAddressLen);
	if (client_fd < 0) // if a new client connected
		throw ServerException("Error. Failed to accept client connection.");
	
	/* pollfd
	is a structure used by poll() to monitor fds for readiness (for reading, writing, or errors)
	It contains:
	struct pollfd {
		int fd	// file descriptor to monitor, 
		int events;	//the events to monitor: POLLIN, POLLOUT
		int revents;	//what actually happened, which is set by poll()
	};
	*/
	pollfd pfd = {0};
	pfd.fd = client_fd;
	pfd.events = POLLIN;
	pfd.revents = 0; // initially no events

	_pollfds.push_back(pfd);

	Client *newClient = new Client(client_fd, clientAddress); // create a new Client object
	_clients.insert(std::make_pair(client_fd, newClient)); // add the new client to the map
}

std::string Server::handleClientlientMessage(int i)
{
	std::string	message;
	
	char 	buffer[MAX_MSG_LEN];
	bzero(buffer, MAX_MSG_LEN);

	
	int bytesReceived = recv(_pollfds[i].fd, buffer, sizeof(buffer) - 1, 0);
	if (bytesReceived > 0)
	{
		buffer[bytesReceived] = '\0';
		std::cout << "Received from client: " << buffer << std::endl;
	}
	else if (bytesReceived == 0)
	{
		std::cout << "Client disconnected: " << _pollfds[i].fd << std::endl;
		// removeClient(_pollfds[i].fd); // WIP
		close(_pollfds[i].fd);
		_pollfds.erase(i);
	}
	else
		std::cerr << RED << "Error receiving data from client. " << DEFAULT << std::endl;
	
}

void Server::handleEvents(void)
{
	pollfd sfd;
	sfd.fd = _server_fd; // server socket
	sfd.events = POLLIN; // listen for incoming connections
	// POLLOUT = ready to write, POLLERR = error on the fd, POLLHUP = client closed socket
	sfd.revents = 0; // initially no events
	
	while (_state == RUNNING) // while server is running
	{
		// Handle client input with poll
		int poll_count = poll(_pollfds, _pollfds.size(), -1);
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
		for (size_t i = 1; i <= MAX_CLIENTS; ++i)
		{
			// Case 1: there was no event with that fd
			if (_pollfds[i].revents == 0)
				continue; // skip to next fd

			// Case 2: the client disconnected
			/*
			Since revents is in binary, we can check for specific events using bitwise AND
			if they have the 1 in common, it will have true as a return
			*/
			if (_pollfds[i].revents & POLLHUP) // if the client closed the connection
			{
				std::cout << GRAY << "Client disconnected: " << _pollfds[i].fd << DEFAULT << std::endl;
				close(_pollfds[i].fd); // close hotel room (socket)
				_clients.erase(_pollfds[i].fd);
				_pollfds.erase(_pollfds.begin() + i);
				continue;
			}

			// Case 3: there was an event with that fd
			if (_pollfds[i].revents & POLLIN) // if there's data to read
			{
				// for the server socket, data to read can be a new client connection
				if (_pollfds[i].fd == _server_fd) // if the server socket is
				{
					handleClientConnections(); // accept new client connection
					continue; // skip to next fd
				}
				
				// for client sockets, data to read can be a message from another client
				handleClientMessage(i);
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
	if (bind(_server_fd, (struct sockaddr *)&_serverAddress, sizeof(_serverAddress)) == -1)
	{
		close(_server_fd);
		throw ServerException("Error. Failed to bind socket.");
	}
	
	// Marks a bound socket as "listening socket"
	// Server is on receiving end of data so needs to listen for incoming connections
	if (listen(_server_fd, SOMAXCONN) == -1) // SOMAXCONN is a constant that defines the maximum number of pending connections in the queue, 128 in most systems
	{
		close(_server_fd);
		throw ServerException("Error. Failed to listen on socket.");
	}
	std::cout << "Server started on port " << _port << std::endl;
	_state = RUNNING; // Server state - 1: running

	/*
	accept is like a reception desk that waits for clients to come in
	when they do, they get a new room (_IRC_client_fd) to communicate with the server
	
	accept blocks until a client connects, unless the server socket is set to non-blocking mode
	for IRC client connection, I left it in blocking mode, so that the server waits for the IRC client (Hexchat) to connect before proceeding
	*/
	_IRC_client_fd = accept(_server_fd, NULL, NULL); // (IRC client = Hexchat)
	if (_IRC_client_fd == -1) 
	{
		std::cerr << RED << "Error. Failed to accept connection." << DEFAULT << std::endl;
		return;
	}

	// subject: "All I/O operations must be non-blocking"
	if (fcntl(_server_fd, F_SETFL, SOCK_NONBLOCK) == -1) // allow server to handle multiple clients at once
	{
		close(_server_fd);
		throw ServerException("Error. Failed to set socket to non-blocking mode.");
	}

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

void toLowercase(const std::string& str)
{
	std::string result = str;
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
	return (result);
}

// For nickname changes within the same Client -- this will allow lower/upper case changes, for example: pia to Pia
bool Server::isNicknameAvailable(const std::string& nickname, const Client* excludeClient) const
{
    std::string lowerNick = nickname;
    toLowercase(lowerNick);
    
    for (std::vector<Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        const Client* client = *it;
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
    // First check format using Client's validation
    if (client->isNickFormatValid(newNickname) != 0)
    {
        // Send format error to client
        return ;
    }
    
    // Then check uniqueness using Server's validation
    if (!isNicknameAvailable(newNickname, client))
    {
        // Send ERR_NICKNAMEINUSE (433) to client
        return ;
    }
    
    // Nickname is valid and available
    client->setNick(newNickname);
}