/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcheron <jcheron@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 12:19:27 by jcheron           #+#    #+#             */
/*   Updated: 2025/05/07 12:02:24 by jcheron          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"
#include <sys/socket.h>
#include <iostream>

Client::Client() : _fd(-1) {}

Client::Client(int fd) : _fd(fd) {}

int Client::getFd() const {
	return _fd;
}

const std::string &Client::getNickname() const {
	return _nickname;
}

const std::string &Client::getUsername() const {
	return _username;
}

const std::string &Client::getHostname() const {
	return _hostname;
}

const std::string &Client::getBuffer() const {
	return _buffer;
}

void Client::setNickname(const std::string &nick) {
	_nickname = nick;
}

void Client::setUsername(const std::string &user) {
	_username = user;
}

void Client::setHostname(const std::string &host) {
	_hostname = host;
}

void Client::appendToBuffeR(const std::string &data) {
	_buffer += data;
}

void Client::clearLineBuffer(size_t len) {
	_buffer.erase(0, len);
}

bool Client::isAuthenticated() const {
	return _authenticated;
}

void Client::setAuthenticated(bool auth) {
	_authenticated = auth;
}

bool Client::isRegistered() const {
	return _authenticated && !_nickname.empty() && !_username.empty();
}

bool Client::hasNick() const { return !_nickname.empty(); }

bool Client::hasUser() const { return !_username.empty(); }

bool Client::isReadyToRegister() const {
	std::cout << "[Check register] auth: " << isAuthenticated()
	          << ", nick: " << hasNick()
	          << ", user: " << hasUser()
	          << ", registered: " << _registered << std::endl;
	return isAuthenticated() && hasNick() && hasUser() && !_registered;
}

void Client::tryRegister() {
	std::cout << "[tryRegister called]" << std::endl;
	if (isReadyToRegister()) {
		_registered = true;
		std::cout << "[Client registered]" << std::endl;
		std::string welcome = ":ft_irc 001 " + _nickname + " :Welcome to the ft_irc server\r\n";
		send(_fd, welcome.c_str(), welcome.size(), 0);
	}
}