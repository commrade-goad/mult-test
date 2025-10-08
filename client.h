#pragma once

class Client {
    public:
        const char *ip;
        int port;
        void *game;
        int sockudp;
        int socktcp;

        Client(const char *ip, int port, void *g): ip(ip), port(port), game(g) {};
        ~Client();
        bool connect_to_server();
        void loop();
};
