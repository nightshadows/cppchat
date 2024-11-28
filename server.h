#pragma once

#include "base.h"

class ChatServer : public ChatBase {
private:
    int client_sock = -1;

    void run_server_loop();

public:
    ChatServer() {};
    virtual ~ChatServer();
    void run();
};
