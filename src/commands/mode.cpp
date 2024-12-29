/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 23:22:55 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/30 02:45:18 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/irc.hpp"

void    modeCommand(
    const std::vector<std::string>& command,
    int client_fd,
    std::map<int, Client>& clients,
    std::map<std::string, std::set<int>>& channels,
    std::map<std::string, std::set<char>>& channelModes
)
{
if (command[0] == "MODE") {
    if (!clients[client_fd].isOperator) {
        sendMessage(client_fd, "ERROR :You must be an operator to use this command.", clients);
        return;
    }

    if (command.size() < 3) {
        sendMessage(client_fd, "ERROR :MODE command requires a channel and a mode (+i or -i).", clients);
        return;
    }

    std::string channel_name = command[1];
    std::string mode = command[2];

    // Check if the channel exists
    if (channels.find(channel_name) == channels.end()) {
        sendMessage(client_fd, "ERROR :Channel " + channel_name + " does not exist.", clients);
        return;
    }

    // Initialize channelModes if not already initialized
    if (channelModes.find(channel_name) == channelModes.end()) {
        channelModes[channel_name] = {};
    }

    // Validate and apply the mode
    if (mode == "+i") {
        if (channelModes[channel_name].count('i') == 0) {
            channelModes[channel_name].insert('i');
            sendMessage(client_fd, "NOTICE AUTH :Invite-only mode enabled for " + channel_name, clients);
            std::cout << "\033[35m[INFO] : Invite-only mode enabled for channel " << channel_name << "\033[0m" << std::endl;
        } else {
            sendMessage(client_fd, "ERROR :Invite-only mode is already enabled for " + channel_name, clients);
        }
    } else if (mode == "-i") {
        if (channelModes[channel_name].count('i') > 0) {
            channelModes[channel_name].erase('i');
            sendMessage(client_fd, "NOTICE AUTH :Invite-only mode disabled for " + channel_name, clients);
            std::cout << "\033[35m[INFO] : Invite-only mode disabled for channel " << channel_name << "\033[0m" << std::endl;
        } else {
            sendMessage(client_fd, "ERROR :Invite-only mode is not enabled for " + channel_name, clients);
        }
    } else {
        sendMessage(client_fd, "ERROR :Invalid mode. Supported modes: +i, -i.", clients);
    }
}
}