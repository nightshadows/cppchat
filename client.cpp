/*
    This is the client class for the 1-1 chat app.
    It handles the client side of the chat - connects to the server and sends messages to it.
*/

#include "client.h"
#include "message.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>

ChatClient::~ChatClient() {
}

void ChatClient::run_client_loop() {
    handle_connection(sockfd);
}

void ChatClient::run(const std::string& server_address) {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(this->port);

    struct hostent* host = gethostbyname(server_address.c_str());
    if (host == nullptr) {
        throw std::runtime_error("Failed to resolve hostname");
    }
    memcpy(&server_addr.sin_addr, host->h_addr, host->h_length);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        throw std::runtime_error("Connection failed");
    }

    std::cout << "Connected to server" << std::endl;

    receive_thread = std::thread([this]() { run_client_loop(); });

    std::string input;
    while (running && std::getline(std::cin, input)) {
        if (!running) break;
        try {
            send_message(input, sockfd);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            break;
        }
    }
}
