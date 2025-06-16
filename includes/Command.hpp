/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cpoulain <cpoulain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 11:21:35 by jcheron           #+#    #+#             */
/*   Updated: 2025/06/16 16:23:53 by cpoulain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include "Client.hpp"
#include <vector>
#include "MessageHelper.hpp"

class Server;

namespace CommandHandler {
	void handleCommand(const std::string &line, Client &client, Server &server);
	void handleNick(const std::vector<std::string>&, Client&, Server&);
	void handleUser(const std::vector<std::string>&, Client&, Server&);
	void handlePing(const std::vector<std::string>&, Client&, Server&);
	void handlePass(const std::vector<std::string> &params, Client &client, Server &server);

	// Channel
	void handleJoin(const std::vector<std::string> &params, Client &client, Server &server);
	void handlePrivmsg(const std::vector<std::string> &params, Client &client, Server &server);
	void handleTopic(const std::vector<std::string> &params, Client &client, Server &server);
	void handleKick(const std::vector<std::string> &params, Client &client, Server &server);
	void handleWhois(const std::vector<std::string> &params, Client &client, Server &server);
	void handleLeave(const std::vector<std::string> &params, Client &client, Server &server);
	void handleQuit(const std::vector<std::string> &params, Client &client, Server &server);
	void handleModes(const std::vector<std::string> &params, Client &client, Server &server);
	void handleInvite(const std::vector<std::string> &params, Client &client, Server &server);
	typedef void (*CommandFunc)(const std::vector<std::string>&, Client&, Server&);
}
