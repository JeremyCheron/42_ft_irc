//
// Created by Eliam on 10/06/2025.
//

#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include "Client.hpp"

class Channel {
private:
	std::string					_topic;
	std::string					_identifier;
	std::map<Client *, bool>	_clients;
	std::set<char>				_modes;
	int							_userLimit;
	std::string					_key;
	std::set<Client *>			_invitedClients;

public:
	// ─────────────────────────────────── Constructor ─────────────────────────────────────
	Channel(const std::string &identifier, const std::string &topic);

	// ──────────────────────────────────── Destructor ─────────────────────────────────────
	~Channel();

	// ────────────────────────────────────── Getters ──────────────────────────────────────
	std::string getTopic() const;
	const std::string getIdentifier() const;

	// ────────────────────────────────────── Setters ──────────────────────────────────────
	void setTopic(std::string topic);

	// ────────────────────────────────────── Methods ──────────────────────────────────────
	void addClient(Client *client, bool isOperator);

	void removeClient(Client *client);

	void broadcast(const std::string& message, Client* sender);

	std::map<Client *, bool> const &getClients() const;

	Client *getClientByNick(const std::string& nickname);

/* -------------------------------------------------------------------------- */
/*                                    MODE                                    */
/* -------------------------------------------------------------------------- */

	void				setMode(char mode);
	void				unsetMode(char mode);
	bool				hasMode(char mode) const;

/* -------------------------------------------------------------------------- */
/*                                    LIMIT                                   */
/* -------------------------------------------------------------------------- */

	void				setUserLimit(int limit);
	void				unsetUserLimit();
	int					getUserLimit() const;
	bool				isChannelFull() const;

/* -------------------------------------------------------------------------- */
/*                                   INVITE                                   */
/* -------------------------------------------------------------------------- */

	void				inviteClient(Client *client);
	void				removeInvitation(Client *client);
	bool				isClientInvited(Client *client) const;

/* -------------------------------------------------------------------------- */
/*                                     KEY                                    */
/* -------------------------------------------------------------------------- */

	void				setKey(const std::string &key);
	void				unsetKey();
	bool				checkKey(const std::string &key) const;
	bool				hasKey() const;
	const std::string	&getKey() const;

/* -------------------------------------------------------------------------- */
/*                                  OPERATORS                                 */
/* -------------------------------------------------------------------------- */

	void				addOperator(Client *client);
	void				removeOperator(Client *client);
	bool				isClientOperator(Client *client) const;

};

