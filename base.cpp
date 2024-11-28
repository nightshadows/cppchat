#include "base.h"
#include "message.h"

#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

const int ChatBase::PORT = 14000;
std::atomic<bool> ChatBase::running{true};
const char ChatBase::ack[] = "message received";
ChatBase* ChatBase::instance = nullptr;

ChatBase::ChatBase() {
    instance = this;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket creation error: " << strerror(errno) << std::endl;
        throw std::runtime_error("Socket creation failed");
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Setsockopt error: " << strerror(errno) << std::endl;
        throw std::runtime_error("setsockopt failed");
    }

    setup_signal_handlers();
}

ChatBase::~ChatBase() {
    running.store(false);

    if (sockfd >= 0) {
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
        sockfd = -1;
    }

    if (receive_thread.joinable()) {
        receive_thread.join();
    }

    std::cout << "Cleanup completed.\n";
}

void ChatBase::send_ack(int socket, uint32_t message_id) {
    Message ack_message(MessageType::ACK, message_id);
    auto serialized = ack_message.serialize();
    if (send(socket, serialized.data(), serialized.size(), 0) < 0) {
        throw std::runtime_error("Failed to send ack");
    }
}

void ChatBase::handle_connection(int sock) {
    while (running) {
        std::vector<uint8_t> buffer(sizeof(MessageHeader));
        ssize_t header_bytes = recv(sock, buffer.data(), sizeof(MessageHeader), MSG_WAITALL);
        if (header_bytes <= 0 || !running) break;

        if (header_bytes != static_cast<ssize_t>(sizeof(MessageHeader))) {
            std::cerr << "Incomplete header received\n";
            continue;
        }

        Message msg;
        msg.deserializeHeader(buffer.data(), sizeof(MessageHeader));

        // Read the data if present
        if (msg.header.data_size > 0) {
            std::vector<uint8_t> buffer(msg.header.data_size);
            ssize_t data_bytes = recv(sock, buffer.data(), msg.header.data_size, MSG_WAITALL);
            if (data_bytes < static_cast<ssize_t>(msg.header.data_size)) {
                std::cerr << "Incomplete data received\n";
                continue;
            }

            msg.deserializeData(buffer.data(), msg.header.data_size);
        }

        if (msg.header.type == MessageType::DATA) {
            std::cout << msg.header.message_id
                    << ": " << msg.data << std::endl;
            if (!running) break;
            send_ack(sock, msg.header.message_id);
        } else if (msg.header.type == MessageType::ACK) {
            std::cout << "Got ack for message " << msg.header.message_id << std::endl;
        }
    }
}

void ChatBase::signal_handler(int sig) {
    std::cout << "\nReceived signal " << sig << ", shutting down...\n" << std::flush;
    running.store(false);

    if (instance && instance->sockfd >= 0) {
        shutdown(instance->sockfd, SHUT_RDWR);
        close(instance->sockfd);
        instance->sockfd = -1;
    }
}

void ChatBase::setup_signal_handlers() {
    struct sigaction sa = {};
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, nullptr) == -1 ||
        sigaction(SIGTERM, &sa, nullptr) == -1) {
        throw std::runtime_error("Failed to set signal handlers");
    }
}

void ChatBase::send_message(const std::string& text, int socket) {
    if (text.length() > Message::MAX_DATA_SIZE) {
        std::cerr << "Message too long (max " << Message::MAX_DATA_SIZE << " bytes)\n";
        return;
    }

    Message msg(MessageType::DATA, get_next_message_id(), text);
    auto serialized = msg.serialize();
    if (send(socket, serialized.data(), serialized.size(), 0) < 0) {
        throw std::runtime_error("Failed to send message");
    }
}

uint32_t ChatBase::get_next_message_id() {
    return next_message_id++;
}
