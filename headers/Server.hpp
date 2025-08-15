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
# include <sys/socket.h>

# include "Client.hpp"
# include "Channel.hpp"
# include "Replies.hpp"

# define DEFAULT	"\x1b[0m"	// for standard output
# define RED		"\x1b[31m" 	// for errors
# define YELLOW 	"\x1b[33m" 	// for important information
# define GRAY 		"\x1b[90m" 	// for debug information

# define MAX_MSG_LEN 512
# define MAX_CHAN_NUM 100
# define MAXCONN 100

class Channel;
class Client;

class Server
{
	public:
		Server(const std::string &portString, const std::string &password);
		~Server(void);

		// Setters & Getters

		void		setName(std::string name);
		std::string	getName(void) const;
		std::string getPassword(void) const;
		sockaddr_in getServerAddress(void) const; // bc client will need it to connect to server
		Client 		*getClient(std::string nickname) const;
		Channel 	*getChannel(const std::string &channel_name) const;

		// Server action functions

		void 		run(void);
		void 		setupSignals(void);
		void 		handleEvents(void);
		void 		handleClientConnections(void);
		void 		handleClientDisconnections(int i);
		void		handleSendingToClient(int i);
		void 		handleClientMessage(int clientFd);
		void 		sendMessageToClient(int clientFD, std::string message);
		void		sendMessageToChannel(Channel* channel, const std::string& message);
		void		sendMessageToChannel(Client* excludedSender, Channel* channel, const std::string& message);
		void		createChannel(std::string &newChannelName, Client &founder);
		void 		removeChannel(Channel *channel);

		// Parser functions

		void		handleInput(Client &client, std::string input);
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

		// Utility functions

		void		parseStringToVector(std::string string, std::vector<std::string> *vector, const char *delimiters);
		size_t		getStringPosition(std::string &string, size_t numberOfArguments);
		bool 		isNicknameAvailable(const std::string& nickname, const Client* targetClient) const;

		// Exception

		class ServerException: public std::exception
		{
			public:
				ServerException(const std::string &message);
				virtual ~ServerException() throw();
				virtual const char* what() const throw();

			private:
				const std::string _message;
		};

	private:
		std::string					_name;
		const unsigned int 			_port;
		const std::string			_password;
		int							_serverFd;
		struct sockaddr_in 			_serverAddress;
		std::vector<pollfd>			_pollfds;
		std::map<int, Client *>		_clients;
		std::vector<Channel *>		_channels;
		std::map<int, std::string>	_incomingMessages;
		std::map<int, std::string>	_outgoingMessages;	
};

void 		signalHandler(int sig);
std::string	toLowercase(const std::string &string);
bool		isPositiveNumber(std::string string);

#endif
