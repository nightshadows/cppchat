#pragma once

#include "base.h"

#include <string>

class ChatClient : public ChatBase {
private:
    void run_client_loop();

public:
    ChatClient() {};
    virtual ~ChatClient();
    void run(const std::string& server_address);
};
