/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MessageHelper.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cpoulain <cpoulain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 13:33:23 by cpoulain          #+#    #+#             */
/*   Updated: 2025/06/16 15:39:58 by cpoulain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Messages.hpp"
#include <sstream>
#include "Client.hpp"
#include <netinet/in.h>
#include <vector>

class MessageHelper
{
	private:
		static std::string	terminateString(const std::string &input);
	public:
		static void			sendMsgToClient(Client *client, const std::string &err);
		static std::string	errNeedMoreParams(const std::string &command);
		static std::string	errNotChannelOperator(const std::string &clientNick, const std::string &channelName);
		static std::string	errNoSuchChannel(const std::string &channelName);
		static std::string	errUserNotInChannel(const std::string &nickname, const std::string &channelName);
		static std::string	errUnknownModeError(const std::string &nickname, const char &modeChar);
		static std::string	errInviteOnlyChannel(const std::string &nickname, const std::string &channelName);
		static std::string	errBadChannelKey(const std::string &nickname, const std::string& channelName);
		static std::string	errChannelIsFull(const std::string &nickname, const std::string& channelName);
		static std::string	rplChannelModeIs(const std::string &nickname, const std::string &channelName, const std::string &modes, const std::string &modesParams);
		static std::string	rplChannelModeChange(const std::string &nickname, const std::string &channelName, const std::vector<std::string> &params);
};
