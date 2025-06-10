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

static std::vector<std::string> split(const std::string &str) {
	std::vector<std::string> tokens;
	std::istringstream iss(str);
	std::string token;
	while (iss >> token) {
		tokens.push_back(token);
	}
	return tokens;
}

void CommandHandler::handleCommand(const std::string &line, Client &client, Server &server) {
	std::vector<std::string> tokens = split(line);
	if (tokens.empty()) return;

	std::string cmd = tokens[0];
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

	if (cmd == "NICK")
		handleNick(tokens, client);
	else if (cmd == "USER")
		handleUser(tokens, client);
	else if (cmd == "PASS")
		handlePass(tokens, client, server);
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
	if (params.size() < 2) return;
	client.setNickname(params[1]);
	std::cout << "[NICK set to]: " << params[1] << std::endl;
}

void CommandHandler::handleUser(const std::vector<std::string> &params, Client &client) {
	if (params.size() < 2) return;
	client.setUsername(params[1]);
	std::cout << "[USER set to]: " << params[1] << std::endl;

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
		std::cout << "[PASS accepted]" <<std::endl;
	} else {
		std::cout << "[PASS rejected]" <<std::endl;
		server.rejectClient(client.getFd(), "Incorrect password");
	}
}
