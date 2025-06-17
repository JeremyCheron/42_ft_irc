//
// Created by Eliam on 10/06/2025.
//

#include "../includes/Channel.hpp"

#include <iostream>
#include <sys/socket.h>

// ──────────────────────────────────── Constructor ─────────────────────────────────────

Channel::Channel(const std::string &topic) : _topic(topic)
{
	std::cout << "\033[1;32m[+] Channel créé : \033[1;36m" << topic << "\033[0m" << std::endl;
}

// ──────────────────────────────────── Destructor ─────────────────────────────────────

Channel::~Channel()
{
	std::cout << "\033[1;31m[-] Channel détruit : \033[1;36m" << _topic << "\033[0m" << std::endl;
}

// ────────────────────────────────────── Getters ──────────────────────────────────────

std::string Channel::getTopic() const
{
	return _topic;
}

Client *Channel::getClientByNick(const std::string &nickname)
{
	for (std::map<Client *, bool>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->first->getNickname() == nickname)
			return it->first;
	}
	return NULL;
}

// ────────────────────────────────────── Setters ──────────────────────────────────────

void Channel::setTopic(std::string topic)
{
	_topic = topic;
}

// ────────────────────────────────────── Methods ──────────────────────────────────────

void Channel::addClient(Client *client, bool isOperator)
{
	_clients[client] = isOperator;
	std::cout << (isOperator ? "\033[1;33m[OP+]" : "[+]") << " " << client->getNickname() << " a rejoint " << _topic << std::endl;
}

void Channel::removeClient(Client *client)
{
	_clients.erase(client);
}

const std::map<Client *, bool> &Channel::getClients() const
{
	return _clients;
}

void Channel::broadcast(const std::string &message, Client *sender)
{
	std::cout << "\033[1;33m[DEBUG][BROADCAST]\033[0m sender fd=" << (sender ? sender->getFd() : -1) << std::endl;
	std::cout << "\033[1;33m[DEBUG][BROADCAST]\033[0m Destinataires : ";
	for (std::map<Client *, bool>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		std::cout << "[fd=" << it->first->getFd() << ", nick=" << it->first->getNickname() << "] ";
	}
	std::cout << std::endl;
	for (std::map<Client *, bool>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->first != sender)
		{
			send(it->first->getFd(), message.c_str(), message.length(), 0);
		}
	}
}

/* -------------------------------------------------------------------------- */
/*                                    MODES                                   */
/* -------------------------------------------------------------------------- */

void Channel::setMode(char mode)
{
	_modes.insert(mode);
}

void Channel::unsetMode(char mode)
{
	_modes.erase(mode);
}

bool Channel::hasMode(char mode) const
{
	return _modes.find(mode) != _modes.end();
}

/* -------------------------------------------------------------------------- */
/*                                     KEY                                    */
/* -------------------------------------------------------------------------- */

void Channel::setKey(const std::string &key)
{
	_key = key;
	setMode('k');
}

void Channel::unsetKey()
{
	_key.clear();
	unsetMode('k');
}

bool Channel::checkKey(const std::string &key) const
{
	return _key == key;
}

bool Channel::hasKey() const
{
	return hasMode('k');
}

const std::string &Channel::getKey() const
{
	return _key;
}

/* -------------------------------------------------------------------------- */
/*                                    LIMIT                                   */
/* -------------------------------------------------------------------------- */

void Channel::setUserLimit(int limit)
{
	_userLimit = limit;
	setMode('l');
}

void Channel::unsetUserLimit()
{
	_userLimit = 0;
	unsetMode('l');
}

int Channel::getUserLimit() const
{
	return _userLimit;
}

bool Channel::isChannelFull() const
{
	return (hasMode('l') && static_cast<int>(_clients.size()) >= _userLimit);
}

/* -------------------------------------------------------------------------- */
/*                                  OPERATORS                                 */
/* -------------------------------------------------------------------------- */

void Channel::addOperator(Client *client)
{
	_clients[client] = true;
}

void Channel::removeOperator(Client *client)
{
	_clients[client] = false;
}

bool Channel::isClientOperator(Client *client) const
{
	std::map<Client *, bool>::const_iterator it = _clients.find(client);
	if (it != _clients.end())
	{
		return it->second;
	}
	return false;
}

/* -------------------------------------------------------------------------- */
/*                                 INVITATIONS                                */
/* -------------------------------------------------------------------------- */

void Channel::inviteClient(Client *client)
{
	_invitedClients.insert(client);
}

void Channel::removeInvitation(Client *client)
{
	_invitedClients.erase(client);
}

bool Channel::isClientInvited(Client *client) const
{
	return _invitedClients.find(client) != _invitedClients.end();
}
