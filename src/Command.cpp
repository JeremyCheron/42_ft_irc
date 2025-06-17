/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cpoulain <cpoulain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 11:24:26 by jcheron           #+#    #+#             */
/*   Updated: 2025/06/17 13:40:27 by cpoulain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Server.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unistd.h>

static std::vector<std::string> split(const std::string &str) {
	std::vector<std::string> tokens;
	std::istringstream iss(str);
	std::string token;
	while (iss >> token) {
		tokens.push_back(token);
	}
	return tokens;
}

typedef void (*CommandFunc)(const std::vector<std::string>&, Client&, Server&);

static std::map<std::string, CommandFunc> initCommandMap() {
	std::map<std::string, CommandFunc> m;
	m["NICK"]    = &CommandHandler::handleNick;
	m["USER"]    = &CommandHandler::handleUser;
	m["PASS"]    = &CommandHandler::handlePass;
	m["JOIN"]    = &CommandHandler::handleJoin;
	m["PRIVMSG"] = &CommandHandler::handlePrivmsg;
	m["TOPIC"]   = &CommandHandler::handleTopic;
	m["KICK"]    = &CommandHandler::handleKick;
	m["PING"]    = &CommandHandler::handlePing;
	m["WHOIS"]   = &CommandHandler::handleWhois;
	m["PART"]    = &CommandHandler::handleLeave;
	m["QUIT"]    = &CommandHandler::handleQuit;
	m["MODE"]    = &CommandHandler::handleModes;
	m["INVITE"]  = &CommandHandler::handleInvite;
	m["CAP"]     = &CommandHandler::handleCap;
	return m;
}

void CommandHandler::handleCommand(const std::string &line, Client &client, Server &server) {
	std::vector<std::string> tokens = split(line);
	if (tokens.empty()) return;

	std::string cmd = tokens[0];
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
	static std::map<std::string, CommandFunc> commandMap = initCommandMap();

	std::map<std::string, CommandFunc>::iterator it = commandMap.find(cmd);
	if (it != commandMap.end()) {
		it->second(tokens, client, server);
	} else {
		if (!client.isRegistered()) {
			std::string msg = ":ft_irc 451 * : You have not registered\r\n";
			send(client.getFd(), msg.c_str(), msg.size(), 0);
			return;
		}
		std::cout << "[Unknown command]: " << cmd << std::endl;
	}
}

// void CommandHandler::handleCommand(const std::string &line, Client &client, Server &server) {
// 	std::vector<std::string> tokens = split(line);
// 	if (tokens.empty()) return;

// 	std::string cmd = tokens[0];
// 	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

// 	if (cmd == "NICK")
// 		handleNick(tokens, client);
// 	else if (cmd == "USER")
// 		handleUser(tokens, client);
// 	else if (cmd == "PASS")
// 		handlePass(tokens, client, server);
// 	else if (cmd == "JOIN")
// 		handleJoin(tokens, client, server);
// 	else if (cmd == "PRIVMSG")
// 		handlePrivmsg(tokens, client, server);
// 	else if (cmd == "TOPIC")
// 		handleTopic(tokens, client, server);
// 	else if (cmd == "KICK")
// 		handleKick(tokens, client, server);
// 	else if (cmd == "PING")
// 		handlePing(tokens, client);
// 	else if (cmd == "WHOIS")
// 		handleWhois(tokens, client, server);
// 	else {
// 		if (!client.isRegistered()) {
// 			std::string msg = ":ft_irc 451 * : You have not registered\r\n";
// 			send(client.getFd(), msg.c_str(), msg.size(), 0);
// 			return;
// 		}
// 		std::cout << "[Unknown command]: " << cmd << std::endl;
// 	}
// }

void CommandHandler::handleNick(const std::vector<std::string> &params, Client &client, Server &server) {
	(void)server;
	if (params.size() < 2) return;
	client.setNickname(params[1]);
	std::cout << "\033[1;36m[NICK]\033[0m Changement de pseudo: " << params[1] << std::endl;
}

void CommandHandler::handleUser(const std::vector<std::string> &params, Client &client, Server &server) {
	(void)server;
	if (params.size() < 2) return;
	client.setUsername(params[1]);
	std::cout << "\033[1;36m[USER]\033[0m Changement de username: " << params[1] << std::endl;

	if (client.isRegistered()) {
		std::string welcome = ":ft_irc 001 " + client.getNickname() + " :Welcome to the ft_irc server\r\n";
		send(client.getFd(), welcome.c_str(), welcome.size(), 0);
	}
}

void CommandHandler::handlePass(const std::vector<std::string> &params, Client &client, Server &server) {
	if (params.size() < 2) return;
	if (client.isAuthenticated()) return;

	if (params[1] == server.getPassword()) {
		client.setAuthenticated(true);
		std::cout << "\033[1;32m[PASS]\033[0m Mot de passe accepté" << std::endl;
	} else {
		std::cout << "\033[1;31m[PASS]\033[0m Mot de passe refusé" << std::endl;
		server.rejectClient(client.getFd(), "Incorrect password");
	}
}

void CommandHandler::handleJoin(const std::vector<std::string> &params, Client &client, Server &server) {
	std::cout << "\033[1;34m[JOIN]\033[0m Commande JOIN reçue de " << client.getNickname() << std::endl;
	if (params.size() < 2) return MessageHelper::sendMsgToClient(&client, MessageHelper::errNeedMoreParams("JOIN"));
	if (client.isRegistered()) {
		std::string channel = params[1];
		server.joinChannel(client, channel, params.size() < 3 ? "" : params[2]);
	}
}

void CommandHandler::handlePrivmsg(const std::vector<std::string> &params, Client &client, Server &server) {
	if (params.size() < 3)
		return;

	std::string target = params[1];

	// 🔧 Correction ici : construire le message proprement
	std::string message;
	bool first = true;
	for (size_t i = 2; i < params.size(); ++i) {
		if (first && params[i][0] == ':')
			message += params[i].substr(1);
		else
			message += (first ? "" : " ") + params[i];
		first = false;
	}

	if (target.empty() || message.empty())
		return;

	std::string fullMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getIp()
		+ " PRIVMSG " + target + " :" + message + "\r\n";

	if (target[0] == '#') {
		// MESSAGE À UN CHANNEL
		Channel* channel = server.getChannel(target);
		std::cout << "\033[1;35m[PRIVMSG]\033[0m Message de " << client.getNickname() << " vers " << target << ": " << message << std::endl;
		if (channel && channel->getClients().count(&client)) {
			std::cout << "\033[1;33m[INFO]\033[0m Broadcast du message à " << target << std::endl;
			channel->broadcast(fullMsg, &client);
		} else {
			std::cout << "\033[1;31m[ERROR]\033[0m Channel " << target << " inexistant ou client non membre." << std::endl;
		}
	} else {
		// MESSAGE PRIVÉ À UN UTILISATEUR
		Client* targetClient = server.findClientByNickname(target);
		if (targetClient) {
			std::cout << "\033[1;35m[PRIVMSG]\033[0m MP de " << client.getNickname() << " à " << target << ": " << message << std::endl;
			send(targetClient->getFd(), fullMsg.c_str(), fullMsg.length(), 0);
		} else {
			std::cout << "\033[1;31m[ERROR]\033[0m Utilisateur " << target << " introuvable." << std::endl;
		}
	}
}



void CommandHandler::handleTopic(const std::vector<std::string> &params, Client &client, Server &server) {
	if (params.size() < 2) return;
	std::string channelName = params[1];
	Channel* channel = server.getChannel(channelName);
	if (!channel) {
		std::string msg = ":ft_irc 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		std::cout << "\033[1;31m[TOPIC]\033[0m Channel inexistant: " << channelName << std::endl;
		return;
	}
	if (params.size() == 2) {
		std::string topic = channel->getTopic();
		if (topic.empty()) {
			std::string noTopicMsg = ":ft_irc 331 " + client.getNickname() + " " + channelName + " :No topic is set\r\n";
			send(client.getFd(), noTopicMsg.c_str(), noTopicMsg.size(), 0);
			std::cout << "\033[1;35m[TOPIC]\033[0m Aucun topic défini pour " << channelName << std::endl;
		} else {
			std::string topicMsg = ":ft_irc 332 " + client.getNickname() + " " + channelName + " :" + topic + "\r\n";
			send(client.getFd(), topicMsg.c_str(), topicMsg.size(), 0);
			std::cout << "\033[1;35m[TOPIC]\033[0m Topic actuel de " << channelName << ": '" << topic << "'" << std::endl;
		}
	} else if (channel->hasMode('t') && !channel->isClientOperator(&client)) {
		return MessageHelper::sendMsgToClient(&client, MessageHelper::errNotChannelOperator(client.getNickname(), channelName));
	}
	else {
		std::string newTopic;
		for (size_t i = 2; i < params.size(); ++i) {
			if (i > 2) newTopic += " ";
			newTopic += params[i];
		}
		channel->setTopic(newTopic);
		std::string topicSetMsg = ":ft_irc 332 " + client.getNickname() + " " + channelName + " " + newTopic + "\r\n";
		send(client.getFd(), topicSetMsg.c_str(), topicSetMsg.size(), 0);
		std::cout << "\033[1;35m[TOPIC]\033[0m \033[1;32mTopic modifié\033[0m pour " << channelName << ": '" << newTopic << "' par " << client.getNickname() << std::endl;
	}
}

void CommandHandler::handleKick(const std::vector<std::string> &params, Client &client, Server &server) {
	if (params.size() < 3) return;

	const std::string& channelName = params[1];
	const std::string& targetNick = params[2];
	std::string reason = "No reason given";
	if (params.size() >= 4) {
		reason = params[3];
		for (size_t i = 4; i < params.size(); ++i) {
			reason += " " + params[i];
		}
	}
	Channel *channel = server.getChannel(channelName);
	if (!channel) return;

	if (!channel->isClientOperator(&client)) {
		std::string err = ":ft_irc 482 " + client.getNickname() + " " + channelName + " :You're not channel operator\r\n";
		send(client.getFd(), err.c_str(), err.size(), 0);
		return;
	}

	Client *target = channel->getClientByNick(targetNick);
	if (!target) {
		std::string err = ":ft_irc 441 " + client.getNickname() + " " + targetNick + " " + channelName + " :They aren't on that channel\r\n";
		send(client.getFd(), err.c_str(), err.size(), 0);
		return;
	}

	std::string msg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";
	channel->broadcast(msg, NULL);
	channel->removeClient(target);
}

void CommandHandler::handleModes(const std::vector<std::string> &params, Client &client, Server &server) {
	if (params.size() < 2)
		return MessageHelper::sendMsgToClient(&client, MessageHelper::errNeedMoreParams("MODE"));
	const std::string& channelName = params[1];

	Channel *channel = server.getChannel(channelName);

	if (!channel)
		return MessageHelper::sendMsgToClient(&client, MessageHelper::errNoSuchChannel(channelName));

	if (params.size() == 2)
	{
		std::string	modes = "+";
		std::string modesParams;

		if (channel->hasMode('i')) modes += 'i';
		if (channel->hasMode('t')) modes += 't';
		if (channel->hasMode('k'))
		{
			modes += 'k';
			modesParams += " " + channel->getKey();
		}
		if (channel->hasMode('l'))
		{
			modes += 'l';
			std::ostringstream oss;
			oss << channel->getUserLimit();
			modesParams += " " + oss.str();
		}
		return MessageHelper::sendMsgToClient(&client, MessageHelper::rplChannelModeIs(client.getNickname(), channelName, modes, modesParams));
	}
	if (!channel->isClientOperator(&client))
		return MessageHelper::sendMsgToClient(&client, MessageHelper::errNotChannelOperator(client.getNickname(), channelName));

	std::string	modeString = params[2];
	size_t		nextParamIndex = 3;
	bool		adding = true;

	for (size_t i = 0; i < modeString.length(); i++)
	{
		char	modeChar = modeString[i];

		if (modeChar == '+')
			adding = true;
		else if (modeChar == '-')
			adding = false;
		else if (modeChar == 'i' || modeChar == 't')
		{
			if (adding)
				channel->setMode(modeChar);
			else
				channel->unsetMode(modeChar);
		}

		else if (modeChar == 'k')
		{
			if (adding)
			{
				if (params.size() <= nextParamIndex)
					return MessageHelper::sendMsgToClient(&client, MessageHelper::errNeedMoreParams("MODE"));
				channel->setKey(params[nextParamIndex++]);
			}
			else
				channel->unsetKey();
		}

		else if (modeChar == 'l')
		{
			if (adding)
			{
				if (params.size() <= nextParamIndex)
					return MessageHelper::sendMsgToClient(&client, MessageHelper::errNeedMoreParams("MODE"));
				int limit = atoi(params[nextParamIndex++].c_str());
				channel->setUserLimit(limit);
			}
			else
				channel->unsetUserLimit();
		}
		else if (modeChar == 'o')
		{
			if (params.size() <= nextParamIndex)
				return MessageHelper::sendMsgToClient(&client, MessageHelper::errNeedMoreParams("MODE"));
			std::string nick = params[nextParamIndex++];

			Client *targetClient = channel->getClientByNick(nick);

			if (!targetClient)
				return MessageHelper::sendMsgToClient(&client, MessageHelper::errUserNotInChannel(client.getNickname(), channelName));
			if (adding)
				channel->addOperator(targetClient);
			else
				channel->removeOperator(targetClient);
		}
		else
			return MessageHelper::sendMsgToClient(&client, MessageHelper::errUnknownModeError(client.getNickname(), modeChar));
	}

	std::vector<std::string> modeParams(params.begin() + 2, params.end());
	std::string usermask = client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname();
	std::string modeChangeMsg = MessageHelper::rplChannelModeChange(usermask, channelName, modeParams);
	channel->broadcast(modeChangeMsg, NULL);
}

void CommandHandler::handlePing(const std::vector<std::string> &params, Client &client, Server &server) {
	if (params.size() < 2) return;
	(void)server;
	std::string pong = "PONG " + params[1] + "\r\n";
	send(client.getFd(), pong.c_str(), pong.size(), 0);
	std::cout << "[PING received, sent PONG]" << std::endl;
}

void CommandHandler::handleWhois(const std::vector<std::string> &params, Client &client, Server &server) {
	if (params.size() < 2) return;

	std::string targetNick = params[1];
	Client* target = server.findClientByNickname(targetNick);
	if (!target) return;

	std::string rpl311 = ":ft_irc 311 " + client.getNickname() + " " + target->getNickname()
		+ " " + target->getUsername() + " localhost * :" + target->getUsername() + "\r\n";
	send(client.getFd(), rpl311.c_str(), rpl311.size(), 0);

	std::string rpl318 = ":ft_irc 318 " + client.getNickname() + " " + target->getNickname()
		+ " :End of WHOIS list\r\n";
	send(client.getFd(), rpl318.c_str(), rpl318.size(), 0);

	std::cout << "[WHOIS for " << targetNick << "]" << std::endl;
}

void	CommandHandler::handleInvite(const std::vector<std::string> &params, Client &client, Server &server)
{
	if (params.size() < 3) return MessageHelper::sendMsgToClient(&client, MessageHelper::errNeedMoreParams("INVITE"));
	std::string	targetNick = params[1];
	std::string	channelName = params[2];

	Channel	*channel = server.getChannel(channelName);
	if (!channel) return MessageHelper::sendMsgToClient(&client, MessageHelper::errNoSuchChannel(channelName));
	if (!channel->getClients().count(&client)) return MessageHelper::sendMsgToClient(&client, MessageHelper::errNotOnChannel(channelName));
	if (channel->hasMode('i') && !channel->isClientOperator(&client)) return MessageHelper::sendMsgToClient(&client, MessageHelper::errNotChannelOperator(client.getNickname(), channelName));

	Client	*targetClient = server.findClientByNickname(targetNick);
	if (!targetClient) return MessageHelper::sendMsgToClient(&client, MessageHelper::errNoSuchNick(targetNick));
	if (channel->getClients().count(targetClient)) return MessageHelper::sendMsgToClient(&client, MessageHelper::errUserOnChannel(targetNick, channelName));

	channel->inviteClient(targetClient);
	MessageHelper::sendMsgToClient(targetClient, MessageHelper::msgInvite(client.getNickname(), targetNick, channelName));
	MessageHelper::sendMsgToClient(&client, MessageHelper::rplChannelInvite(client.getNickname(), targetNick, channelName));
}

void CommandHandler::handleCap(const std::vector<std::string> &params, Client &client, Server &server)
{
	if (params.size() < 2) return MessageHelper::sendMsgToClient(&client, MessageHelper::errNeedMoreParams("CAP"));
	(void)server;

	std::string subCommand = params[1];

	if (subCommand == "LS")
	{
		std::string nick = client.isRegistered() ? client.getNickname() : "*";
		return MessageHelper::sendMsgToClient(&client, MessageHelper::msgCapLS(nick, ""));

	} else if (subCommand == "END")
	{
		return ; // Do nothing for END
	} else
	{
		std::string nick = client.isRegistered() ? client.getNickname() : "*";
		return MessageHelper::sendMsgToClient(&client, MessageHelper::errInvalidCapSubCommand(nick, subCommand));
	}
}

void CommandHandler::handleLeave(const std::vector<std::string> &params, Client &client, Server &server) {
	if (params.size() < 2) {
		std::string msg = ":ft_irc 461 " + client.getNickname() + " " + "" + " :Not enough parameters\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}
	const std::string& channelName = params[1];
	Channel *channel = server.getChannel(channelName);
	if (!channel) {
		std::string msg = ":ft_irc 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}
	if (channel->getClients().count(&client) == 0) {
		std::string msg = ":ft_irc 442 " + client.getNickname() + " " + channelName + " :You're not on that channel\r\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
		return;
	}

	channel->removeClient(&client);
	std::string leaveMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PART " + channelName + " :Leaving\r\n";
	channel->broadcast(leaveMsg, &client);
	send(client.getFd(), leaveMsg.c_str(), leaveMsg.size(), 0);
	std::cout << "[LEAVE] Client " << client.getNickname() << " has left channel " << channelName << std::endl;
}



void CommandHandler::handleQuit(const std::vector<std::string> &params, Client &client, Server &server) {
	(void)params;
    const std::map<std::string, Channel *>& channels = server.getChannelsMap();
    for (std::map<std::string, Channel *>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
        Channel *channel = it->second;
        if (channel->getClients().count(&client)) {
            channel->removeClient(&client);
            std::string partMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PART " + channel->getTopic() + "\r\n";
            channel->broadcast(partMsg, &client);
        }
    }
    std::string msg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost QUIT :Client disconnected\r\n";
    send(client.getFd(), msg.c_str(), msg.size(), 0);
    std::cout << "[QUIT] Client " << client.getNickname() << " has disconnected." << std::endl;
    close(client.getFd());
}
