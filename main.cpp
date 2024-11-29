#include "server.h"
#include "client.h"

#include <getopt.h>
#include <iostream>

int main(int argc, char* argv[]) {
    static struct option long_options[] = {
        {"server", no_argument,       nullptr, 's'},
        {"client", required_argument, nullptr, 'c'},
        {"port",   required_argument, nullptr, 'p'},
        {"help",   no_argument,       nullptr, 'h'},
        {nullptr,  0,                 nullptr,  0 }
    };

    try {
        bool is_server = false;
        std::string server_address;
        bool show_help = false;
        bool invalid_args = false;
        uint16_t port = 0;

        int opt;
        while ((opt = getopt_long(argc, argv, "sc:hp:", long_options, nullptr)) != -1) {
            switch (opt) {
                case 's':
                    is_server = true;
                    break;
                case 'c':
                    is_server = false;
                    server_address = optarg;
                    break;
                case 'p':
                    port = std::atoi(optarg);
                    break;
                case 'h':
                    show_help = true;
                    break;
                default:
                    invalid_args = true;
                    break;
            }
        }

        if (show_help || invalid_args || (!is_server && server_address.empty())) {
            std::cout << "Usage:" << std::endl;
            std::cout << "  Server mode: " << argv[0] << " -s or --server [--port <port>]" << std::endl;
            std::cout << "  Client mode: " << argv[0] << " -c <server_address> or --client <server_address> [--port <port>]" << std::endl;
            return show_help ? 0 : 1;
        }

        if (is_server) {
            ChatServer chat;
            if (port != 0) {
                chat.set_port(port);
            }
            chat.run();
        } else {
            ChatClient chat;
            if (port != 0) {
                chat.set_port(port);
            }
            chat.run(server_address);
        }
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
