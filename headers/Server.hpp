/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shaintha <shaintha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 15:53:47 by shaintha          #+#    #+#             */
/*   Updated: 2025/07/09 15:53:47 by shaintha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <cstdlib>
# include <cstring>
# include <sstream>
# include <algorithm>
# include <vector>
# include <map>
# include <signal.h>
# include <netinet/in.h> // for sockaddr_in
# include <poll.h>

# include "Client.hpp"
# include "Channel.hpp"
# include "Replies.hpp"

//# include <iomanip>
//# include <sys/socket.h> // for socket, bind, listen, accept
//# include <fcntl.h> // to set socket to non-blocking mode
//# include <errno.h> // - number of last error
//# include <unistd.h>
//# include <exception>
//# include <string>
//# include <stdio.h>
//# include <cstddef>
//# include <climits>

# define DEFAULT "\x1b[0m" // for standard output
# define RED "\x1b[31m" // for errors
# define YELLOW "\x1b[33m" // for warnings
# define GRAY "\x1b[90m" // for debug information

# define MAX_MSG_LEN 512
# define MAX_CHAN_NUM 100

// Server states
# define RUNNING 1
# define NOT_RUNNING 0
# define ERROR -1

class Channel;
class Client;

class Server
{
	public:
		Server(const std::string &portString, const std::string &password);
		~Server(void);


		// Signals
		void setupSignals();
		
		// Setter & Getters
		void		setName(std::string name);
		std::string	getName(void) const;
		std::string getPassword(void) const;
		Channel 	*getChannel(const std::string &channel_name) const;
		sockaddr_in getServerAddress(void) const; // bc client will need it to connect to server
		int 		getState(void) const;
		Client 		*getClient(std::string nickname) const;

		// Member functions - server actions
		void 		handleClientConnections(void); // like addClient
		void		handleSendingToClient(int i);
		void 		sendMessageToClient(int clientFD, std::string message);
		void		sendMessageToChannel(Channel* channel, const std::string& message);
		void		sendMessageToChannel(Client* excludedSender, Channel* channel, const std::string& message);
		void 		handleClientMessage(int clientFd);
		void 		handleClientDisconnections(int i);  // like removeClient
		void 		handleEvents(void);
		void 		run(void);

		// Member functions - user triggered actions
		void		createChannel(std::string &newChannelName, Client &founder);
		void 		removeChannel(Channel *channel);

		// Nickname availability checks
		bool 		isNicknameAvailable(const std::string& nickname, const Client* targetClient) const;
		void 		handleNickCommand(Client* client, const std::string& newNickname);

		// PARSER
		void		handleInput(Client &client, std::string input);
		void		parseStringToVector(std::string input, std::vector<std::string> *vector, const char *delimiters);
		size_t		getInputPosition(std::string &input, size_t numberOfArguments);
		void		executeCommand(Client &client, std::vector<std::string> command, std::string &input);
		int			pass(Client &client, std::vector<std::string> command, size_t cmdNumber);
		int			nick(Client &client, std::vector<std::string> command, size_t cmdNumber);
		int			user(Client &client, std::vector<std::string> command, std::string &input, size_t cmdNumber);
		int			join(Client &client, std::vector<std::string> command, size_t cmdNumber);
		int			part(Client &client, std::vector<std::string> command, std::string &input, size_t cmdNumber);
		int			invite(Client &client, std::vector<std::string> command, size_t cmdNumber);
		int			kick(Client &client, std::vector<std::string> command, std::string &input, size_t cmdNumber);
		int			privMsg(Client &client, std::vector<std::string> command, std::string input, size_t cmdNumber);
		int			topic(Client &client, std::vector<std::string> command, std::string &input, size_t cmdNumber);
		int			mode(Client &client, std::vector<std::string> command, size_t cmdNumber);
		int 		quit(Client &client, std::vector<std::string> command, size_t cmdNumber);

		// Exception
		class ServerException: public std::exception
		{
			public:
				ServerException(const std::string &message);
				virtual const char* what() const throw();
				virtual ~ServerException() throw();

			private:
				const std::string _message;
		};

	private:
		Server(void);
		Server(const Server &other);
		Server					&operator=(const Server &other);
	
		std::string				_name;
		int						_serverFd; // Server Socket FD - Listening socket, can be negative
		const unsigned int 		_port; // Port number - "door to the server"
		struct sockaddr_in 		_serverAddress; // for IPv4 - holds network info - like IP address and port number - that the server uses to know where to listen or connect
		// struct sockaddr_in6 	_serverAddress; // for IPv6 - holds network info - like IP address and port number - that the server uses to know where to listen or connect
		/*
		server creates sockaddr_in "serverAddress" to specify its own IP address and port to bind to
		client takes this sockaddr_in "serverAddress" to specify the server's IP address and port to connect to
		*/
		
		const std::string			_password;
		std::map<int, Client *>		_clients;	// List of connected clients (ClientClass objs)
		std::vector<pollfd>			_pollfds; // +1 for the server socket
		std::map<int, std::string>	_outgoingMessages; // Buffer for outgoing messages
		std::map<int, std::string>	_incomingMessages; // Buffer for incoming messages
		std::vector<Channel *>		_channels;	// List of channels (ChannelClass objs)
		//std::vector<std::string>	_users; // auf 10 users limitieren

		// clients must be unique within a channel

		int						_state; // Server state - 0: not running, 1: running, -1: error (?)_

		
};

void 		signalHandler(int sig);
std::string	toLowercase(const std::string& str);
bool		isPositiveNumber(std::string string);

#endif
