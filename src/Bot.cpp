/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cpoulain <cpoulain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 15:07:03 by cpoulain          #+#    #+#             */
/*   Updated: 2025/06/17 15:45:23 by cpoulain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Bot.hpp"

#include <algorithm>

Bot::Bot()
{
	_forbiddenWords.insert("tig");
	_forbiddenWords.insert("espmer");
	_forbiddenWords.insert("pech'app");
}

Bot::~Bot()
{
	_forbiddenWords.clear();
	_warnings.clear();
}

void Bot::handleMessage(Client *client, Channel *channel, const std::string &message)
{
	std::string lowerMessage = message;
	std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(), ::tolower);

	for (size_t i = 0; i < lowerMessage.size(); i++)
		if (!isalnum(lowerMessage[i]) && !isspace(lowerMessage[i]))
			lowerMessage[i] = ' ';

	std::istringstream iss(lowerMessage);
	std::string word;

	while (iss >> word)
	{
		if (!word.empty() && _forbiddenWords.find(word) != _forbiddenWords.end())
		{
			if (_warnings[client] == 0)
			{
				sendWarning(client, channel);
				_warnings[client]++;
			}
			else
			{
				kickClient(client, channel);
				_warnings.erase(client);
			}
			return;
		}
	}
}

void Bot::sendWarning(Client *client, Channel *channel)
{
	std::string message = ":" + client->getNickname() + " PRIVMSG " + channel->getIdentifier() + " :" + MSG_WARN + "\r\n";

	const std::map<Client *, bool> &clients = channel->getClients();
	for (std::map<Client *, bool>::const_iterator it = clients.begin(); it != clients.end(); ++it)
	{
		Client *currentClient = it->first; // Key is the Client*
		MessageHelper::sendMsgToClient(currentClient, message);
	}
}

void Bot::kickClient(Client *client, Channel *channel)
{
	if (!channel->getClients().count(client))
		return;

	std::string botNickname = "Bot";
	std::string kickMessage = ":" + botNickname + " KICK " + channel->getIdentifier() + " " + client->getNickname() + " :You have been kicked for inappropriate language.\r\n";

	/* Envoyer le message de kick à tous les clients du canal */
	const std::map<Client *, bool> &clients = channel->getClients();
	for (std::map<Client *, bool>::const_iterator it = clients.begin(); it != clients.end(); ++it)
	{
		Client *currentClient = it->first; // Key is the Client*
		MessageHelper::sendMsgToClient(currentClient, kickMessage);
	}

	/* Retirer le client du canal */
	channel->removeClient(client);
}
