/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcheron <jcheron@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 12:20:23 by jcheron           #+#    #+#             */
/*   Updated: 2025/05/07 11:51:53 by jcheron          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>
#include <vector>
#include <poll.h>
#include <netinet/in.h>

class Client;

class Server {

	public:

		Server(int port, const std::string &password);
		~Server();

		std::string getPassword() const;
		void rejectClient(int clientFd, const std::string &reason);

		void run();

	private:

		int _serverSocket;
		int _port;
		std::string _password;
		std::vector<pollfd> _pollFds;
		std::map<int, Client> _clients;

		void setupSocket();
		void acceptClient();
		void handleClientMessage(int clientFd);
		void disconnectClient(int clientFd);

};
