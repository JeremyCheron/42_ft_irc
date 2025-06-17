/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cpoulain <cpoulain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 12:20:23 by jcheron           #+#    #+#             */
/*   Updated: 2025/06/17 15:44:16 by cpoulain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>
#include <vector>
#include <poll.h>
#include <netinet/in.h>
#include "Channel.hpp"
#include "Client.hpp"
#include <csignal>
#include "Bot.hpp"

class Client;

class Server {

	public:

		Server(int port, const std::string &password);
		~Server();

		std::string getPassword() const;
		void rejectClient(int clientFd, const std::string &reason);
		void run();
		void shutdown();

		void joinChannel(Client& client, const std::string& string, const std::string &key);
		Channel * getChannel(std::string string);
		Client* findClientByNickname(const std::string& nickname);
		std::map<std::string, Channel*> getChannelsMap();

	private:

		static Server	*instance;
		static void signalHandler(int signum);

		Bot	_bot;
		int _serverSocket;
		int _port;
		std::string _password;
		std::vector<pollfd> _pollFds;
		std::map<int, Client *> _clients;
		std::map<std::string, Channel*> _channelsMap;

		void setupSocket();
		void setupSignalHandler();
		void acceptClient();
		void handleClientMessage(int clientFd);
		void disconnectClient(int clientFd);
		Channel *getChannelFromMessage(const std::string &message);
};
