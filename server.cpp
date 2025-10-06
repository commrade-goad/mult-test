#include <iostream>

#include "server.h"

Server::Server(const char *ip, int port) {
    this->ip = ip;
    this->port = port;
}
Server::~Server() {
    close(this->server_fd);
    close(this->epoll_fd);
}

void Server::start() {
    this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->server_fd == -1) {
        perror("socket");
        return;
    }

    int opt = 1;
    setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(this->port);
        if (bind(this->server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(this->server_fd);
        return;
    }

    if (listen(this->server_fd, SOMAXCONN) < 0) {
        perror("listen");
        close(this->server_fd);
        return;
    }

    int flags = fcntl(this->server_fd, F_GETFL, 0);
    fcntl(this->server_fd, F_SETFL, flags | O_NONBLOCK);

    this->epoll_fd = epoll_create1(0);
    if (this->epoll_fd == -1) {
        perror("epoll_create1");
        close(this->server_fd);
        return;
    }
    epoll_event event{};
    event.data.fd = this->server_fd;
    event.events = EPOLLIN;
    epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->server_fd, &event);

    this->events.reserve(MAX_EVENTS);
    this->_start_loop();
}

void Server::_start_loop() {
    while (true) {
        int n = epoll_wait(epoll_fd, events.data(), MAX_EVENTS, -1);
        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == server_fd) {
                // New client connection
                sockaddr_in client_addr{};
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
                if (client_fd != -1) {
                    int flags = fcntl(client_fd, F_GETFL, 0);
                    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
                    epoll_event client_event{};
                    client_event.data.fd = client_fd;
                    client_event.events = EPOLLIN;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);
                    std::cout << "New client connected, fd=" << client_fd << "\n";
                }
            } else {
                int client_fd = events[i].data.fd;
                char buffer[1024];
                ssize_t count = read(client_fd, buffer, sizeof(buffer) - 1);
                if (count <= 0) {
                    std::cout << "Client disconnected, fd=" << client_fd << "\n";
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
                    close(client_fd);
                } else {
                    buffer[count] = '\0';
                    std::cout << "Client " << client_fd << ": " << buffer;
                    std::string reply = "Echo: " + std::string(buffer);
                    send(client_fd, reply.c_str(), reply.size(), 0);
                }
            }
        }
    }
}
