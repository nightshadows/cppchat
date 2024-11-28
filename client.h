#pragma once

#include "base.h"

#include <iostream>
#include <string>
#include <cstring>
#include <chrono>
#include <thread>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <poll.h>

class ChatClient : public ChatBase {
private:
    void run_client_loop();

public:
    ChatClient() {};
    virtual ~ChatClient();
    void run(const std::string& server_address);
};
