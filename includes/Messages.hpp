/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Messages.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cpoulain <cpoulain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 13:36:13 by cpoulain          #+#    #+#             */
/*   Updated: 2025/06/13 16:24:47 by cpoulain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

const size_t		max_length = 510;
const std::string	serverName = "ft_irc";

const std::string	ERR_NEEDMOREPARAMS = " 461 ";
const std::string	MSG_NEEDMOREPARAMS = " :Not enough parameters";

const std::string	ERR_CHANOPRIVSNEEDED = " 482 ";
const std::string	MSG_CHANOPRIVSNEEDED = " :You're not channel operator";

const std::string	ERR_NOSUCHCHANNEL = " 403 ";
const std::string	MSG_NOSUCHCHANNEL = " :No such channel";

const std::string	RPL_CHANNELMODEIS = " 324 ";
