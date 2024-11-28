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

class ChatServer : public ChatBase {
private:
    int client_sock = -1;

    void run_server_loop();

public:
    ChatServer() {};
    virtual ~ChatServer();
    void run();
};
