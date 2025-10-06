#include <cstring>
#include <string>
#include "window.h"
#include "server.h"

int main(int argc, char **argv) {
    std::string ip = "0.0.0.0";
    int port = 4269;
    bool server = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-ip") == 0 && i + 1 < argc)   ip     = argv[++i];
        if (strcmp(argv[i], "-port") == 0 && i + 1 < argc) port   = atoi(argv[++i]);
        if (strcmp(argv[i], "-server") == 0)               server = true;
    }
    if (!server) {
        Window w(1280, 720, "hello");
        w.start();
    } else {
        Server s(ip.c_str(), port);
        s.start();
    }
    return 0;
}
