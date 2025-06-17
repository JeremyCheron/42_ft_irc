/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MessageHelper.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cpoulain <cpoulain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 13:42:16 by cpoulain          #+#    #+#             */
/*   Updated: 2025/06/17 14:34:50 by cpoulain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/MessageHelper.hpp"

void MessageHelper::sendMsgToClient(Client *client, const std::string &err)
{
	send(client->getFd(), err.c_str(), err.size(), 0);
}

std::string MessageHelper::terminateString(const std::string &input)
{
	if (input.length() < max_length)
	{
		return input.substr(0, max_length) + "\r\n";
	}
	return input + "\r\n";
}

std::string MessageHelper::errNeedMoreParams(const std::string &command)
{
	std::ostringstream oss;
	oss << ":" << serverName << ERR_NEEDMOREPARAMS << command << MSG_NEEDMOREPARAMS;
	return terminateString(oss.str());
}

std::string MessageHelper::errNotChannelOperator(const std::string &clientNick, const std::string &channelName)
{
	std::ostringstream oss;
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

std::string MessageHelper::errNotOnChannel(const std::string &channelName)
{
	std::ostringstream oss;
	oss << ":" << serverName << ERR_NOTONCHANNEL << channelName << MSG_NOTONCHANNEL;
	return terminateString(oss.str());
}

std::string MessageHelper::errUserOnChannel(const std::string &targetNick, const std::string &channelName)
{
	std::ostringstream oss;
	oss << ":" << serverName << ERR_USERONCHANNEL << targetNick << " " << channelName << MSG_USERONCHANNEL;
	return terminateString(oss.str());
}

std::string MessageHelper::errNoSuchNick(const std::string &targetNick)
{
	std::ostringstream oss;
	oss << ":" << serverName << ERR_NOSUCHNICK << targetNick << MSG_NOSUCHNICK;
	return terminateString(oss.str());
}

std::string MessageHelper::errUnknownModeError(const std::string &nickname, const char &modeChar)
{
	std::ostringstream oss;
	oss << ":" << serverName << ERR_UNKNOWNMODE << nickname << " " << modeChar << MSG_UNKNOWNMODE;
	return terminateString(oss.str());
}

std::string MessageHelper::msgInvite(const std::string &nickname, const std::string &targetNick, const std::string &channelName)
{
	std::ostringstream oss;
	oss << ":" << nickname << " INVITE " << targetNick << " :" << channelName;
	return terminateString(oss.str());
}

std::string MessageHelper::rplChannelInvite(const std::string &nickname, const std::string &targetNick, const std::string &channelName)
{
	std::ostringstream oss;
	oss << ":" << serverName << RPL_INVITING << nickname << " " << targetNick << " :" << channelName;
	return terminateString(oss.str());
}

std::string MessageHelper::rplChannelModeChange(const std::string &nickname, const std::string &channelName, const std::vector<std::string> &params)
{
	std::ostringstream oss;
	oss << ":" << nickname << " MODE " << channelName;
	for (size_t i = 0; i < params.size(); i++)
		oss << " " << params[i];
	return terminateString(oss.str());
}

std::string MessageHelper::errInviteOnlyChannel(const std::string &nickname, const std::string &channelName)
{
	std::ostringstream oss;
	oss << ":" << serverName << ERR_INVITEONLYCHAN << nickname << " " << channelName << MSG_INVITEONYLCHAN;
	return terminateString(oss.str());
}

std::string MessageHelper::errBadChannelKey(const std::string &nickname, const std::string &channelName)
{
	std::ostringstream oss;
	oss << ":" << serverName << ERR_BADCHANNELKEY << nickname << " " << channelName << MSG_BADCHANNELKEY;
	return terminateString(oss.str());
}

std::string MessageHelper::errChannelIsFull(const std::string &nickname, const std::string &channelName)
{
	std::ostringstream oss;
	oss << ":" << serverName << ERR_CHANNELISFULL << nickname << " " << channelName << MSG_CHANNELISFULL;
	return terminateString(oss.str());
}

std::string MessageHelper::errInvalidCapSubCommand(const std::string &nickname, const std::string &sub)
{
	std::ostringstream oss;
	oss << ":" << serverName << ERR_INVALIDCAPCMD << nickname << " " << sub << MSG_INVALIDCAPCMD;
	return terminateString(oss.str());
}

std::string MessageHelper::msgCapLS(const std::string &nickname, const std::string &caps)
{
	std::ostringstream oss;
	oss << ":" << serverName << " CAP " << nickname << " LS :" << caps;
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
