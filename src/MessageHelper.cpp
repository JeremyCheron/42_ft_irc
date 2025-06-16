/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MessageHelper.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cpoulain <cpoulain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 13:42:16 by cpoulain          #+#    #+#             */
/*   Updated: 2025/06/16 14:56:50 by cpoulain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/MessageHelper.hpp"

void		MessageHelper::sendMsgToClient(Client *client, const std::string &err)
{
	send(client->getFd(), err.c_str(), err.size(), 0);
}

std::string MessageHelper::terminateString(const std::string &input)
{
	if (input.length() < max_length) {
		return input.substr(0, max_length) + "\r\n";
	}
	return input + "\r\n";
}

std::string MessageHelper::errNeedMoreParams(const std::string &command)
{
	std::ostringstream	oss;
	oss << ":" << serverName << ERR_NEEDMOREPARAMS << command << MSG_NEEDMOREPARAMS;
	return terminateString(oss.str());
}

std::string MessageHelper::errNotChannelOperator(const std::string &clientNick, const std::string &channelName)
{
	std::ostringstream	oss;
	oss << ":" << serverName << ERR_CHANOPRIVSNEEDED << clientNick << " " << channelName << MSG_CHANOPRIVSNEEDED;
	return terminateString(oss.str());
}

std::string MessageHelper::errNoSuchChannel(const std::string &channelName)
{
	std::ostringstream oss;
	oss << ":" << serverName << ERR_NOSUCHCHANNEL << channelName << MSG_NOSUCHCHANNEL;
	return terminateString(oss.str());
}

std::string MessageHelper::errUserNotInChannel(const std::string &nickname, const std::string &channelName)
{
	std::ostringstream oss;
	oss << ":" << serverName << ERR_USERNOTINCHANNEL << nickname << " " << channelName << MSG_USERNOTINCHANNEL;
	return terminateString(oss.str());
}

std::string MessageHelper::errUnknownModeError(const std::string &nickname, const char &modeChar)
{
	std::ostringstream oss;
	oss << ":" << serverName << ERR_UNKNOWNMODE << nickname << " " << modeChar << MSG_UNKNOWNMODE;
	return terminateString(oss.str());
}

std::string	MessageHelper::rplChannelModeChange(const std::string &nickname, const std::string &channelName, const std::vector<std::string> &params)
{
	std::ostringstream oss;
	oss << ":" << nickname << " MODE " << channelName;
	for (size_t i = 0; i < params.size(); i++)
		oss << " " << params[i];
	return terminateString(oss.str());
}

std::string MessageHelper::rplChannelModeIs(const std::string &nickname, const std::string &channelName, const std::string &modes, const std::string &modesParams)
{
	std::ostringstream oss;
	oss << ":" << serverName << RPL_CHANNELMODEIS << nickname << " " << channelName << " " << modes;
	if (!modesParams.empty())
		oss << modesParams;
	return terminateString(oss.str());
}
