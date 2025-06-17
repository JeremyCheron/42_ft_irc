/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Messages.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cpoulain <cpoulain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 13:36:13 by cpoulain          #+#    #+#             */
/*   Updated: 2025/06/17 13:23:06 by cpoulain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

const size_t		max_length				= 510;
const std::string	serverName				= "ft_irc";

const std::string	ERR_NEEDMOREPARAMS		= " 461 ";
const std::string	MSG_NEEDMOREPARAMS		= " :Not enough parameters";

const std::string	ERR_CHANOPRIVSNEEDED	= " 482 ";
const std::string	MSG_CHANOPRIVSNEEDED	= " :You're not channel operator";

const std::string	ERR_NOSUCHNICK			= " 401 ";
const std::string	MSG_NOSUCHNICK			= " :No such nick/channel";

const std::string	ERR_NOSUCHCHANNEL		= " 403 ";
const std::string	MSG_NOSUCHCHANNEL		= " :No such channel";

const std::string	ERR_USERNOTINCHANNEL	= " 441 ";
const std::string	MSG_USERNOTINCHANNEL	= " :They aren't on that channel";

const std::string	ERR_NOTONCHANNEL		= " 442 ";
const std::string	MSG_NOTONCHANNEL		= " :You're not on that channel";

const std::string	ERR_USERONCHANNEL		= " 443 ";
const std::string	MSG_USERONCHANNEL		= " :is already on channel";

const std::string	ERR_UNKNOWNMODE			= " 472 ";
const std::string	MSG_UNKNOWNMODE			= " :is unknown mode char to me";

const std::string	ERR_INVITEONLYCHAN		= " 473 ";
const std::string	MSG_INVITEONYLCHAN		= " :Cannot join channel (+i)";

const std::string	ERR_BADCHANNELKEY		= " 475 ";
const std::string	MSG_BADCHANNELKEY		= " :Cannot join channel (+k)";

const std::string	ERR_CHANNELISFULL		= " 471 ";
const std::string	MSG_CHANNELISFULL		= " :Cannot join channel (+l)";

const std::string	ERR_INVALIDCAPCMD		= " 410 ";
const std::string	MSG_INVALIDCAPCMD		= " :Invalid CAP subcommand";

const std::string	RPL_CHANNELMODEIS		= " 324 ";

const std::string	RPL_INVITING			= " 341 ";
