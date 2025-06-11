/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcheron <jcheron@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 11:24:26 by jcheron           #+#    #+#             */
/*   Updated: 2025/05/07 12:05:05 by jcheron          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Server.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

struct IRCMessage {
	std::string prefix;
	std::string command;
	std::vector<std::string> params;
};

IRCMessage parseIRCLine(const std::string &line) {
	IRCMessage msg;
	std::istringstream iss(line);
	std::string token;

	if (!line.empty() && line[0] == ':') {
		iss >> token;
		msg.prefix = token.substr(1);
	}

	if (!(iss >> msg.command))
		return msg;

	while (iss >> token) {
		if (token[0] == ':') {
			std::string trailing = token.substr(1);
			std::string word;
			while (iss >> word)
				trailing += " " + word;
			msg.params.push_back(trailing);
			break;
		}
		msg.params.push_back(token);
	}
	return msg;
}

void handlePlaceholder(const std::string &cmd, const std::vector<std::string> &params, Client &client) {
	std::string response;

	if (cmd == "MODE") {
		response = ":ft_irc 324 " + client.getNickname() + " +i\r\n";
	} else if (cmd == "WHOIS") {
		std::string target = (params.empty() ? client.getNickname() : params[0]);
		response = ":ft_irc 311 " + client.getNickname() + " " + target + " 127.0.0.1 * :Realname\r\n";
		response += ":ft_irc 318 " + client.getNickname() + " " + target + " :End of /WHOIS list\r\n";
	} else {
		response = ":ft_irc 421 " + client.getNickname() + " " + cmd + " :Unknown command\r\n";
	}

	send(client.getFd(), response.c_str(), response.size(), 0);
}

// static std::vector<std::string> split(const std::string &str) {
// 	std::vector<std::string> tokens;
// 	std::istringstream iss(str);
// 	std::string token;
// 	while (iss >> token) {
// 		tokens.push_back(token);
// 	}
// 	return tokens;
// }

void CommandHandler::handleCommand(const std::string &line, Client &client, Server &server) {
	// std::vector<std::string> tokens = split(line);
	// if (tokens.empty()) return;

	IRCMessage msg = parseIRCLine(line);
	if (msg.command.empty())
		return;

	std::string cmd = msg.command;
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

	if (cmd == "NICK")
		handleNick(msg.params, client);
	else if (cmd == "USER")
		handleUser(msg.params, client);
	else if (cmd == "PASS")
		handlePass(msg.params, client, server);
	else if (cmd == "JOIN")
		handleJoin(msg.params, client, server);
	else if (cmd == "PRIVMSG")
		handlePrivmsg(msg.params, client, server);
	else if (cmd == "PING")
		send(client.getFd(), ("PONG :" + (msg.params.empty() ? "ft_irc" : msg.params[0]) + "\r\n").c_str(), line.length(), 0);
	else if (cmd == "MODE" || cmd == "WHOIS")
		handlePlaceholder(cmd, msg.params, client);
	else {
		if (!client.isRegistered()) {
			std::string msg = ":ft_irc 451 * : You have not registered\r\n";
			send(client.getFd(), msg.c_str(), msg.size(), 0);
			return;
		}
		std::cout << "[Unknown command]: " << cmd << std::endl;
	}
}

void CommandHandler::handleNick(const std::vector<std::string> &params, Client &client) {
	if (params.empty()) return;
	client.setNickname(params[0]);
	std::cout << "[NICK set to]: " << params[0] << std::endl;
	client.tryRegister();
}

void CommandHandler::handleUser(const std::vector<std::string> &params, Client &client) {
	if (params.empty()) return;
	client.setUsername(params[0]);
	std::cout << "[USER set to]: " << params[0] << std::endl;

	client.tryRegister();
}

void CommandHandler::handlePass(const std::vector<std::string> &params, Client &client, Server &server) {
	if (params.empty()) return;
	if (client.isAuthenticated()) return;

	if (params[0] == server.getPassword()) {
		client.setAuthenticated(true);
		std::cout << "[PASS accepted]" <<std::endl;
		client.tryRegister();
	} else {
		std::cout << "[PASS rejected]" <<std::endl;
		server.rejectClient(client.getFd(), "Incorrect password");
	}
}

void CommandHandler::handleJoin(const std::vector<std::string> &params, Client &client, Server &server) {
	std::cout << "[JOIN command received]" << std::endl;
	if (params.size() < 2) return;
	if (client.isRegistered()) {
		std::string channel = params[1];
		server.joinChannel(client, channel);
	}
}

void CommandHandler::handlePrivmsg(const std::vector<std::string> &params, Client &client, Server &server) {
	if (params.size() < 3) return;
	std::string target = params[1];
	std::string message;
	// message
	for (size_t i = 2; i < params.size(); ++i) {
		if (i > 2) message += " ";
		message += params[i];
	}
	if (!target.empty() && !message.empty()) {
		Channel* channel = server.getChannel(target);
		std::cout << "[PRIVMSG to " << target << "]: " << message << std::endl;
		if (channel && channel->getClients().count(&client)) {
			std::cout << "[Broadcasting message to channel " << target << "]" << std::endl;
			std::string msg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PRIVMSG " + target + " :" + message + "\r\n";
			channel->broadcast(msg, &client);
		}
	}
}
