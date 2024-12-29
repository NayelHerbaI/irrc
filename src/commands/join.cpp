/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nherbal <nherbal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/28 23:06:05 by nherbal           #+#    #+#             */
/*   Updated: 2024/12/30 02:49:46 by nherbal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/irc.hpp"

void joinCommand(
    const std::vector<std::string>& command,
    const std::string& server_password,
    std::map<int, bool>& isAuthenticated,
    std::map<int, Client>& clients,
    int client_fd,
    std::map<std::string, std::set<int>>& channels,
    std::map<std::string, std::set<int>>& inviteList,  // now by reference
    std::map<std::string, std::set<char>>& channelModes // now by reference
)
{
if (command[0] == "JOIN") {
    if (command.size() > 1) {
        std::string channel_name = command[1];

        // Validate that the channel name starts with #
        if (channel_name[0] != '#') {
            sendMessage(client_fd, "ERROR :Invalid channel name. Channel names must start with #", clients);
            return;
        }

        // Check if the channel exists
        auto channel_it = channels.find(channel_name);
        bool is_new_channel = (channel_it == channels.end());

        // Handle new channel creation
        if (is_new_channel) {
            // Create the channel and initialize its modes
            channels[channel_name] = {};        // Initialize the channel
            channelModes[channel_name] = {};   // Initialize the channel's modes

            // Log the creation of the new channel
            std::cout << "\033[35m[INFO] : Created new channel: " << channel_name << "\033[0m" << std::endl;
        }

        // Handle invite-only mode
        if (channelModes[channel_name].count('i')) { // Check if +i mode is active
            if (inviteList[channel_name].count(client_fd) == 0) { // Client must be in the invite list
                sendMessage(client_fd, "ERROR :You must be invited to join " + channel_name, clients);
                std::cout << "\033[31m[ERROR] : Client " << client_fd << " denied access to invite-only channel "
                          << channel_name << " due to lack of invitation.\033[0m" << std::endl;
                return;
            }
            // Remove the client from the invite list after they join
            inviteList[channel_name].erase(client_fd);
            std::cout << "\033[35m[INFO] : Client " << client_fd << " removed from invite list for channel "
                      << channel_name << "." << std::endl;
        }

        // Add the client to the channel
        channels[channel_name].insert(client_fd);

        // Add the channel to the client's joined channels
        clients[client_fd].joinedChannels.insert(channel_name);

        // Notify the client about the successful join
        sendMessage(client_fd, ":" + clients[client_fd].nickname + " JOIN " + channel_name, clients);
        std::cout << "\033[35m[INFO] : Client " << client_fd << " joined channel: " << channel_name << "\033[0m" << std::endl;

        // Notify other members in the channel
        for (int member_fd : channels[channel_name]) {
            if (member_fd != client_fd) {
                sendMessage(member_fd, ":" + clients[client_fd].nickname + " JOIN " + channel_name, clients);
            }
        }
    } else {
        sendMessage(client_fd, "ERROR :No channel name provided", clients);
    }
}

}