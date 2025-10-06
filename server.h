#pragma once
#include <cstdio>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <vector>

#define MAX_EVENTS 10

class Server {
    public:
        const char * ip;
        int port;

        int server_fd, epoll_fd;
        std::vector<epoll_event> events;

        Server(const char *ip, int port);
        ~Server();
        void start();
        void _start_loop();
};
