// simple irc client 

// Client main
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring> // BUGFIX: allowed?

#include "../headers/Server.hpp"
#include "../headers/Parser.hpp"
#include "../headers/Client.hpp"

int main(void)
{
	Client client;

	client.setNick("julianwad");
	if (client.connectToServer("127.0.0.1", 6667) == -1)
	{
		std::cerr << "Failed to connect to server" << std::endl;
		return (1);
	}

	client.setNick("julian");
	if (!client.getNickname().empty())
		std::cout << "Nick: " << client.getNickname() << std::endl;

	std::string userName = "Julian";
    std::string realName = "Julian Uitz";

	client.setUser(userName, 0, '*', realName);
		std::cout << "User: " << client.getUsername() << "\n" << "Real name: " << client.getRealname() << std::endl;

    send(client.getSocketFD(), "JOIN #test\r\n", 13, 0); // Example command to join a channel
    
    return (0);
}