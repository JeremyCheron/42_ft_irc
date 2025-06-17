/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cpoulain <cpoulain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 15:04:18 by cpoulain          #+#    #+#             */
/*   Updated: 2025/06/17 15:44:06 by cpoulain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <sys/socket.h>
#include "Client.hpp"
#include "Channel.hpp"
#include "MessageHelper.hpp"

const std::string MSG_WARN = "WARNING: Inappropriate language detected. Further violations will result in a kick.";

class Client;
class Channel;
class MessageHelper;

class Bot
{
	public:
		Bot();
		~Bot();
		void handleMessage(Client *client, Channel *channel, const std::string &message);
		void sendWarning(Client *client, Channel *channel);
		void kickClient(Client *client, Channel *channel);

	private:
		std::set<std::string> _forbiddenWords;
		std::map<Client *, int> _warnings;
};
