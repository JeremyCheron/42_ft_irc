//
// Created by Eliam on 10/06/2025.
//

#include "../includes/Channel.hpp"

#include <iostream>
#include <sys/socket.h>
#include <__ostream/basic_ostream.h>

// ──────────────────────────────────── Constructor ─────────────────────────────────────

Channel::Channel(const std::string &topic) : _topic(topic) {
	std::cout << "+ Channel created :" << topic << std::endl;
}

// ──────────────────────────────────── Destructor ─────────────────────────────────────

Channel::~Channel() {
	std::cout << "- Channel destroyed :" << _topic << std::endl;
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
	std::cout << "[+] " << client->getNickname() << " joined " << _topic << std::endl;
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
	for (std::map<Client*, bool>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->first != sender) {
			send(it->first->getFd(), message.c_str(), message.length(), 0);
		}
	}
}
