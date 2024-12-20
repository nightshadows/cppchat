/*
    This is the server class for the 1-1 chat app.
    It handles the server side of the chat - main difference from the client is that it listens for connections.
*/

#include "server.h"
#include "message.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>

ChatServer::~ChatServer() {
    if (client_sock >= 0) {
        shutdown(client_sock, SHUT_RDWR);
        close(client_sock);
        client_sock = -1;
    }
}

void ChatServer::run_server_loop() {
    while (running) {
        client_sock = accept(sockfd, nullptr, nullptr);
        if (!running) {
            if (client_sock >= 0) close(client_sock);
            break;
        }

        if (client_sock < 0) {
            if (running) std::cerr << "Accept failed" << std::endl;
            break;
        }

        std::cout << "Client connected" << std::endl;
        handle_connection(client_sock);
        std::cout << "Client disconnected" << std::endl;
        clear_message_timestamps();
        close(client_sock);
        client_sock = -1;
    }
}

void ChatServer::run() {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(this->port);

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Errno: " << errno << " - " << strerror(errno) << std::endl;
        throw std::runtime_error("Bind failed");
    }

    listen(sockfd, 1);
    std::cout << "Server listening on port " << this->port << std::endl;

    receive_thread = std::thread([this]() { run_server_loop(); });

    std::string input;
    while (running && std::getline(std::cin, input)) {
        if (!running) break;
        if (client_sock >= 0) {
            try {
                send_message(input, client_sock);
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                break;
            }
        } else {
            std::cout << "No client connected. Message not sent." << std::endl;
        }
    }
}
