/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcheron <jcheron@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 12:23:23 by jcheron           #+#    #+#             */
/*   Updated: 2025/05/07 11:53:04 by jcheron          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Command.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

Server::Server(int port, const std::string &password)
	: _port(port), _password(password) {
	setupSocket();
}

Server::~Server() {
	close(_serverSocket);
}

std::string Server::getPassword() const {
	return _password;
}

void Server::setupSocket() {
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket < 0)
		throw std::runtime_error("Socket creation failed");

	int enable = 1;
	setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

	sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(_port);

	if (bind(_serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
		throw std::runtime_error("Bind failed");

	if (listen(_serverSocket, 10) < 0)
		throw std::runtime_error("Listenf failed");

	fcntl(_serverSocket, F_SETFL, O_NONBLOCK);

	pollfd pfd;
	pfd.fd = _serverSocket;
	pfd.events = POLLIN;
	_pollFds.push_back(pfd);

	std::cout << "Server started on port " << _port << std::endl;
}

void Server::run() {
	while (true) {
		int ret = poll(&_pollFds[0], _pollFds.size(), -1);
		if (ret < 0) {
			std::cerr << "Poll error" << std::endl;
			continue;
		}
		for (size_t i = 0; i < _pollFds.size(); ++i) {
			if (_pollFds[i].fd == _serverSocket) {
				acceptClient();
			} else {
				handleClientMessage(_pollFds[i].fd);
			}
		}
	}
}

void Server::acceptClient() {
	sockaddr_in clientAddr;
	socklen_t len = sizeof(clientAddr);
	int clientFd = accept(_serverSocket, (struct sockaddr *)&clientAddr, &len);

	if (clientFd < 0)
		return;

	fcntl(clientFd, F_SETFL, O_NONBLOCK);

	pollfd pfd;
	pfd.fd = clientFd;
	pfd.events = POLLIN;
	_pollFds.push_back(pfd);

	_clients[clientFd] = Client(clientFd);
	std::cout << "New client connected: fd=" << clientFd << std::endl;
}

void Server::handleClientMessage(int clientFd) {
	char buffer[512];
	std::memset(buffer, 0, sizeof(buffer));
	int bytes = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

	if (bytes < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
		std::cerr << "Recv error on fd=" << clientFd << std::endl;
		disconnectClient(clientFd);
		return;
	} else if (bytes == 0) {
		std::cerr << "Client closed connection: fd=" << clientFd << std::endl;
		disconnectClient(clientFd);
		return;
	}
	buffer[bytes] = '\0';
	Client &client = _clients[clientFd];
	client.appendToBuffeR(std::string(buffer));

	std::string &buf = const_cast<std::string &>(client.getBuffer());
	size_t pos;
	while ((pos = buf.find('\n')) != std::string::npos) {
		std::string line = buf.substr(0, pos);
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		std::cout << "Parsed line from fd=" << clientFd << ": [" << line << "]" << std::endl;

		CommandHandler::handleCommand(line, client, *this);

		client.clearLineBuffer(pos + 1);
	}
}

void Server::disconnectClient(int clientFd) {
	std::cout << "Client disconnected: fd=" << clientFd << std::endl;
	close(clientFd);
	_clients.erase(clientFd);

	for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
		if (it->fd == clientFd) {
			_pollFds.erase(it);
			break;
		}
	}
}

void Server::rejectClient(int clientFd, const std::string &reason) {
	std::string message = ":ft_irc 464 * :" + reason + "\r\n";
	send(clientFd, message.c_str(), message.size(), 0);
	disconnectClient(clientFd);
}
