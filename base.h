#pragma once

#include <thread>
#include <atomic>

class ChatBase {
private:
    static ChatBase* instance;
    uint32_t next_message_id = 0;
    static const char ack[];

    void send_ack(int socket, uint32_t message_id);
    static void signal_handler(int sig);
    void setup_signal_handlers();
    void run_loop();
    uint32_t get_next_message_id();

protected:
    static const int PORT;
    int sockfd = -1;
    std::thread receive_thread;
    static std::atomic<bool> running;

    ChatBase();
    virtual ~ChatBase();
    void handle_connection(int sock);
    void send_message(const std::string& text, int socket);
};
