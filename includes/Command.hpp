/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcheron <jcheron@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 11:21:35 by jcheron           #+#    #+#             */
/*   Updated: 2025/05/07 11:44:21 by jcheron          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include "Client.hpp"
#include <vector>

class Server;

namespace CommandHandler {
	void handleCommand(const std::string &line, Client &client, Server &server);
	void handleNick(const std::vector<std::string> &params, Client &client);
	void handleUser(const std::vector<std::string> &params, Client &client);
	void handlePass(const std::vector<std::string> &params, Client &client, Server &server);

	// Channel
	void handleJoin(const std::vector<std::string> &params, Client &client, Server &server);
	void handlePrivmsg(const std::vector<std::string> &params, Client &client, Server &server);
}
