//
// Created by Eliam on 10/06/2025.
//

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include "Client.hpp"

class Channel {
private:
	std::string _topic;
	std::map<Client *, bool> _clients; //bool pour savoir si un user est operator

public:
	// ─────────────────────────────────── Constructor ─────────────────────────────────────
	Channel(const std::string &topic);

	// ──────────────────────────────────── Destructor ─────────────────────────────────────
	~Channel();

	// ────────────────────────────────────── Getters ──────────────────────────────────────
	std::string getTopic() const;

	// ────────────────────────────────────── Setters ──────────────────────────────────────
	void setTopic(std::string topic);

	// ────────────────────────────────────── Methods ──────────────────────────────────────
	void addClient(Client *client, bool isOperator);

	bool isClientOperator(Client *client) const;

	void removeClient(Client *client);

	void broadcast(const std::string& message, Client* sender);

	std::map<Client *, bool> const &getClients() const;

	Client *getClientByNick(const std::string& nickname);

};


#endif //CHANNEL_HPP
