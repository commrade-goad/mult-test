#include <cstring>
#include <string>
#include <thread>
#include "window.h"
#include "server.h"
#include "client.h"

int main(int argc, char **argv) {
    std::string ip = "0.0.0.0";
    int port = 4269;
    bool server = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-ip") == 0 && i + 1 < argc) {
            ip = argv[++i];
            continue;
        }
        if (strcmp(argv[i], "-port") == 0 && i + 1 < argc) {
            port   = atoi(argv[++i]);
            continue;
        }
        if (strcmp(argv[i], "-server") == 0) {
            server = true;
            continue;
        }
    }
    if (!server) {
        // TODO: Send tcp to server to get the id
        Window w(1280, 720, "hello");
        Client c(ip.c_str(), port, (void *)&w.g);
        c.connect_to_server();
        std::thread udp_thread(&Client::loop, &c);
        w.start();
        w.g.exit = true;
        udp_thread.join();
    } else {
        Server s(ip.c_str(), port);
        s.start();
    }
    return 0;
}
