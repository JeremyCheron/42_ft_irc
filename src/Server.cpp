/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cpoulain <cpoulain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 12:23:23 by jcheron           #+#    #+#             */
/*   Updated: 2025/06/17 15:01:18 by cpoulain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Command.hpp"
#include "Channel.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstdio>
#include <cstdlib>

Server* Server::instance = NULL;

Server::Server(int port, const std::string &password)
	: _port(port), _password(password)
{
	instance = this;
	setupSignalHandler();
	setupSocket();
}

Server::~Server()
{
	close(_serverSocket);
}

std::string Server::getPassword() const
{
	return _password;
}

void Server::setupSocket()
{
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

void Server::shutdown()
{
	std::cout << "\033[1;33m[Shutdown]\033[0m Server is shutting down..." << std::endl;

	instance->_pollFds.clear();
	for (std::map<int, Client*>::iterator it = instance->_clients.begin(); it != instance->_clients.end(); ++it)
	{
		int fd = it->first;
		Client* client = it->second;
		std::cout << "Closing client fd=" << fd << std::endl;
		close(fd);
		delete client;
	}
	instance->_clients.clear();

	for (std::map<std::string, Channel*>::iterator it = instance->_channelsMap.begin(); it != instance->_channelsMap.end(); ++it)
	{
		delete it->second;
	}
	instance->_channelsMap.clear();
	if (instance->_serverSocket >= 0)
	{
		close(instance->_serverSocket);
		instance->_serverSocket = -1;
	}
	instance->~Server();
}

void Server::signalHandler(int signum)
{
	std::cout << "\n\033[1;31m[Signal]\033[0m Caught signal " << signum << ", shutting down..." << std::endl;
	if (Server::instance)
		Server::instance->shutdown();
	exit(0);
}

void Server::setupSignalHandler()
{
	struct sigaction sa;
	sa.sa_handler = Server::signalHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	if (sigaction(SIGINT, &sa, NULL) == -1 || sigaction(SIGTERM, &sa, NULL) == -1)
		throw std::runtime_error("Failed to set signal handlers");
}

void Server::run()
{
	while (true)
	{
		int ret = poll(&_pollFds[0], _pollFds.size(), -1);
		if (ret < 0)
		{
			std::cerr << "Poll error" << std::endl;
			continue;
		}
		for (size_t i = 0; i < _pollFds.size(); ++i)
		{
			if (_pollFds[i].fd == _serverSocket)
			{
				acceptClient();
			}
			else
			{
				handleClientMessage(_pollFds[i].fd);
			}
		}
	}
}

void Server::acceptClient()
{
	sockaddr_in clientAddr;
	socklen_t len = sizeof(clientAddr);
	int clientFd = accept(_serverSocket, (struct sockaddr *)&clientAddr, &len);

	if (clientFd < 0)
	{
		perror("accept");
		return;
	}

	fcntl(clientFd, F_SETFL, O_NONBLOCK);

	pollfd pfd;
	pfd.fd = clientFd;
	pfd.events = POLLIN;
	_pollFds.push_back(pfd);

	Client *client = new Client(clientFd);
	std::string ip = inet_ntoa(clientAddr.sin_addr);
	client->setIp(ip);
	_clients[clientFd] = client;
	std::cout << "New client connected: fd=" << clientFd << std::endl;
}

void Server::handleClientMessage(int clientFd)
{
	char buffer[512];
	std::memset(buffer, 0, sizeof(buffer));
	int bytes = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

	if (bytes < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
		std::cerr << "Recv error on fd=" << clientFd << std::endl;
		disconnectClient(clientFd);
		return;
	}
	else if (bytes == 0)
	{
		std::cerr << "Client closed connection: fd=" << clientFd << std::endl;
		disconnectClient(clientFd);
		return;
	}

	buffer[bytes] = '\0';

	Client *client = _clients[clientFd];
	client->appendToBuffeR(std::string(buffer));

	std::string &buf = const_cast<std::string &>(client->getBuffer());
	size_t pos;
	while ((pos = buf.find('\n')) != std::string::npos)
	{
		std::string line = buf.substr(0, pos);
		client->clearLineBuffer(pos + 1);
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);

		std::cout << "Parsed line from fd=" << clientFd << ": [" << line << "]" << std::endl;

		// 🧠 DÉTECTION DCC SEND
		if (line.find("DCC SEND") != std::string::npos)
		{
			std::cout << "\033[1;36m[DCC DETECTÉ]\033[0m : " << line << std::endl;

			// Essai de parsing simple
			size_t dccPos = line.find("DCC SEND");
			std::istringstream iss(line.substr(dccPos));
			std::string dcc, send, filename, ipStr, portStr, sizeStr;
			iss >> dcc >> send >> filename >> ipStr >> portStr >> sizeStr;

			if (!filename.empty() && !ipStr.empty() && !portStr.empty())
			{
				uint32_t ipNum = strtoul(ipStr.c_str(), NULL, 10);
				in_addr ipAddr;
				ipAddr.s_addr = htonl(ipNum);
				std::string ip = inet_ntoa(ipAddr);

				std::cout << "\033[1;35m[FICHIER]\033[0m "
						  << "Nom: " << filename
						  << ", IP: " << ip
						  << ", Port: " << portStr
						  << ", Taille: " << sizeStr << std::endl;
			}
		}

		// traitement normal
		CommandHandler::handleCommand(line, *client, *this);
	}
}

void Server::disconnectClient(int clientFd)
{
	std::cout << "Client disconnected: fd=" << clientFd << std::endl;
	close(clientFd);
	Client *client = _clients[clientFd];
	delete client;
	_clients.erase(clientFd);

	for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it)
	{
		if (it->fd == clientFd)
		{
			_pollFds.erase(it);
			break;
		}
	}
}

void Server::rejectClient(int clientFd, const std::string &reason)
{
	std::string message = ":ft_irc 464 * :" + reason + "\r\n";
	send(clientFd, message.c_str(), message.size(), 0);
	disconnectClient(clientFd);
}

void Server::joinChannel(Client &client, const std::string &string, const std::string &key)
{
	std::cout << "Client " << client.getNickname() << " is trying to join channel: " << string << std::endl;
	Channel *channel = NULL;
	std::map<std::string, Channel *>::iterator it = _channelsMap.find(string);
	if (it != _channelsMap.end())
	{
		channel = it->second;
		if (channel->hasMode('i') && !channel->isClientInvited(&client))
			return MessageHelper::sendMsgToClient(&client, MessageHelper::errInviteOnlyChannel(client.getNickname(), string));
		else if (channel->hasMode('k') && !channel->checkKey(key))
			return MessageHelper::sendMsgToClient(&client, MessageHelper::errBadChannelKey(client.getNickname(), string));
		else if (channel->hasMode('l') && channel->isChannelFull())
			return MessageHelper::sendMsgToClient(&client, MessageHelper::errChannelIsFull(client.getNickname(), string));
		channel->addClient(&client, false);
		channel->removeInvitation(&client);
		std::cout << client.getNickname() << " joined channel " << string << std::endl;
	}
	else
	{
		channel = new Channel(string);
		_channelsMap[string] = channel;
		channel->addClient(&client, true);
		std::cout << client.getNickname() << " created and joined channel " << string << std::endl;
	}

	// Envoi du message JOIN au client
	std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getIp() + " JOIN :" + string +
						  "\r\n";
	send(client.getFd(), joinMsg.c_str(), joinMsg.length(), 0);

	// Envoi du topic
	std::string topic = channel->getTopic();
	if (topic.empty())
	{
		std::string noTopic = ":ft_irc 331 " + client.getNickname() + " " + string + " :No topic is set\r\n";
		send(client.getFd(), noTopic.c_str(), noTopic.length(), 0);
	}
	else
	{
		std::string topicMsg = ":ft_irc 332 " + client.getNickname() + " " + string + " :" + topic + "\r\n";
		send(client.getFd(), topicMsg.c_str(), topicMsg.length(), 0);
	}

	// Envoi de la liste des utilisateurs
	std::string names = ":ft_irc 353 " + client.getNickname() + " = " + string + " :";
	bool first = true;
	for (std::map<Client *, bool>::const_iterator cit = channel->getClients().begin(); cit != channel->getClients().end(); ++cit)
	{
		if (!first)
			names += " ";
		first = false;
		if (channel->isClientOperator(cit->first))
			names += "@";
		names += cit->first->getNickname();
	}
	names += "\r\n";
	send(client.getFd(), names.c_str(), names.length(), 0);
	std::string endNames = ":ft_irc 366 " + client.getNickname() + " " + string + " :End of /NAMES list.\r\n";
	send(client.getFd(), endNames.c_str(), endNames.length(), 0);

	// LOGS AMÉLIORÉS AVEC COULEURS
	std::cout << "\033[1;34m[JOIN]\033[0m Client \033[1;32m" << client.getNickname() << "\033[0m (fd=" << client.getFd() << ") rejoint le channel \033[1;36m" << string << "\033[0m" << std::endl;
	if (it != _channelsMap.end())
	{
		std::cout << "\033[1;33m[INFO]\033[0m Ajout de " << client.getNickname() << " dans le channel existant " << string << std::endl;
	}
	else
	{
		std::cout << "\033[1;33m[INFO]\033[0m Création du channel \033[1;36m" << string << "\033[0m et ajout de " << client.getNickname() << " comme opérateur" << std::endl;
	}

	if (topic.empty())
	{
		std::cout << "\033[1;35m[TOPIC]\033[0m Aucun topic défini pour " << string << std::endl;
	}
	else
	{
		std::cout << "\033[1;35m[TOPIC]\033[0m Topic de " << string << ": '" << topic << "'" << std::endl;
	}

	// Envoi de la liste des utilisateurs
	std::cout << "\033[1;36m[USERS]\033[0m Liste des utilisateurs dans " << string << ": ";
	for (std::map<Client *, bool>::const_iterator cit = channel->getClients().begin(); cit != channel->getClients().end(); ++cit)
	{
		if (channel->isClientOperator(cit->first))
			std::cout << "@";
		std::cout << cit->first->getNickname();
		if (++std::map<Client *, bool>::const_iterator(cit) != channel->getClients().end())
			std::cout << ", ";
	}
	std::cout << std::endl;
}

Channel *Server::getChannel(std::string string)
{
	std::map<std::string, Channel *>::iterator it = _channelsMap.find(string);
	if (it != _channelsMap.end())
	{
		return it->second;
	}
	return NULL;
}

Client *Server::findClientByNickname(const std::string &nickname)
{
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
			return it->second;
	}
	return NULL;
}

std::map<std::string, Channel *> Server::getChannelsMap()
{
	return _channelsMap;
}
