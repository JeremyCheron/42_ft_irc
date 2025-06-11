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

#include <iostream>

Client::Client() : _fd(-1) {
	std::cout << "\033[1;32m[+] Client créé (fd=" << _fd << ")\033[0m" << std::endl;
}

Client::Client(int fd) : _fd(fd) {
	std::cout << "\033[1;32m[+] Client créé (fd=" << fd << ")\033[0m" << std::endl;
}

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
	std::cout << "\033[1;36m[NICK]\033[0m Changement de pseudo: " << nick << std::endl;
}

void Client::setUsername(const std::string &user) {
	_username = user;
	std::cout << "\033[1;36m[USER]\033[0m Changement de username: " << user << std::endl;
}

void Client::setHostname(const std::string &host) {
	_hostname = host;
	std::cout << "\033[1;36m[HOST]\033[0m Changement de hostname: " << host << std::endl;
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
	std::cout << (auth ? "\033[1;32m[AUTH]\033[0m Authentifié" : "\033[1;31m[AUTH]\033[0m Non authentifié") << std::endl;
}

bool Client::isRegistered() const {
	return _authenticated && !_nickname.empty() && !_username.empty();
}
