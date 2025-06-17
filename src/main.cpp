/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cpoulain <cpoulain@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 12:25:39 by jcheron           #+#    #+#             */
/*   Updated: 2025/06/17 14:34:49 by cpoulain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cstdlib>
#include <iostream>

int main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	int port = std::atoi(av[1]);
	if (port <= 0 || port > 65535)
	{
		std::cerr << "Error: Invalid port number." << std::endl;
		return 1;
	}

	std::string password = av[2];
	try
	{
		Server server(port, password);
		server.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << "server error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
