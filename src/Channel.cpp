//
// Created by Eliam on 10/06/2025.
//

#include "../includes/Channel.hpp"

#include <iostream>
#include <sys/socket.h>

// ──────────────────────────────────── Constructor ─────────────────────────────────────

Channel::Channel(const std::string &topic) : _topic(topic) {
	std::cout << "\033[1;32m[+] Channel créé : \033[1;36m" << topic << "\033[0m" << std::endl;
}

// ──────────────────────────────────── Destructor ─────────────────────────────────────

Channel::~Channel() {
	std::cout << "\033[1;31m[-] Channel détruit : \033[1;36m" << _topic << "\033[0m" << std::endl;
}

// ────────────────────────────────────── Getters ──────────────────────────────────────

std::string Channel::getTopic() const {
	return _topic;
}

// ────────────────────────────────────── Setters ──────────────────────────────────────

void Channel::setTopic(std::string topic) {
	_topic = topic;
}

// ────────────────────────────────────── Methods ──────────────────────────────────────

void Channel::addClient(Client *client, bool isOperator) {
	_clients[client] = isOperator;
	std::cout << (isOperator ? "\033[1;33m[OP+]" : "[+]") << " " << client->getNickname() << " a rejoint " << _topic << std::endl;
}

bool Channel::isClientOperator(Client *client) const {
	std::map<Client *, bool>::const_iterator it = _clients.find(client);
	if (it != _clients.end()) {
		return it->second;
	}
	return false;
}

void Channel::removeClient(Client *client) {
	_clients.erase(client);
}

const std::map<Client *, bool> &Channel::getClients() const {
	return _clients;
}

void Channel::broadcast(const std::string& message, Client* sender) {
	std::cout << "\033[1;33m[DEBUG][BROADCAST]\033[0m sender fd=" << (sender ? sender->getFd() : -1) << std::endl;
	std::cout << "\033[1;33m[DEBUG][BROADCAST]\033[0m Destinataires : ";
	for (std::map<Client*, bool>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
		std::cout << "[fd=" << it->first->getFd() << ", nick=" << it->first->getNickname() << "] ";
	}
	std::cout << std::endl;
	for (std::map<Client*, bool>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->first != sender) {
			send(it->first->getFd(), message.c_str(), message.length(), 0);
		}
	}
}
