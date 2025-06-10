/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcheron <jcheron@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 12:16:45 by jcheron           #+#    #+#             */
/*   Updated: 2025/05/07 11:58:49 by jcheron          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class Client {

	public:
		Client();
		Client(int fd);

		int getFd() const;
		const std::string &getNickname() const;
		const std::string &getUsername() const;
		const std::string &getHostname() const;
		const std::string &getBuffer() const;

		void setNickname(const std::string &nick);
		void setUsername(const std::string &user);
		void setHostname(const std::string &host);
		void appendToBuffeR(const std::string &data);
		void clearLineBuffer(size_t len);

		bool isAuthenticated() const;
		void setAuthenticated(bool auth);

		bool isRegistered() const;

	private:

		int _fd;
		std::string _nickname;
		std::string _username;
		std::string _hostname;
		std::string _buffer;
		bool _authenticated;
};
