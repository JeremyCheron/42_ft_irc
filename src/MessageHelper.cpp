/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MessageHelper.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cpoulain <cpoulain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 13:42:16 by cpoulain          #+#    #+#             */
/*   Updated: 2025/06/13 16:24:41 by cpoulain         ###   ########.fr       */
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

std::string MessageHelper::rplChannelModeIs(const std::string &nickname, const std::string &channelName, const std::string &modes, const std::string &modesParams)
{
	std::ostringstream oss;
	oss << ":" << serverName << RPL_CHANNELMODEIS << nickname << " " << channelName << " " << modes;
	if (!modesParams.empty())
	{
		oss << " " << modesParams;
	}
	return terminateString(oss.str());
}
