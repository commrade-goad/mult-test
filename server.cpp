#include <cstring>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include "server.h"
#include "data.h"

Server::Server(const char *ip, int port) {
    this->ip = ip;
    this->port = port;
}

Server::~Server() {
    close(this->server_fd);
    close(this->udp_fd);
    close(this->epoll_fd);
}

void Server::start() {
    // === TCP socket setup ===
    this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->server_fd == -1) {
        perror("TCP socket");
        return;
    }

    int opt = 1;
    setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(this->port);

    if (bind(this->server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind TCP");
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

    // === UDP socket setup ===
    this->udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (this->udp_fd == -1) {
        perror("UDP socket");
        close(this->server_fd);
        return;
    }

    setsockopt(this->udp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(this->udp_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind UDP");
        close(this->udp_fd);
        close(this->server_fd);
        return;
    }

    flags = fcntl(this->udp_fd, F_GETFL, 0);
    fcntl(this->udp_fd, F_SETFL, flags | O_NONBLOCK);

    // === epoll setup ===
    this->epoll_fd = epoll_create1(0);
    if (this->epoll_fd == -1) {
        perror("epoll_create1");
        close(this->server_fd);
        close(this->udp_fd);
        return;
    }

    epoll_event event{};

    // Add TCP listening socket
    event.data.fd = this->server_fd;
    event.events = EPOLLIN;
    epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->server_fd, &event);

    // Add UDP socket
    event.data.fd = this->udp_fd;
    event.events = EPOLLIN;
    epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->udp_fd, &event);

    std::cout << "[INFO] Server started on port " << this->port << "\n";
    this->_start_loop();
}

void Server::_start_loop() {
    while (true) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;

            if (fd == server_fd) {
                // --- TCP: accept new client ---
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

                    std::cout << "[TCP] New client connected, fd=" << client_fd << "\n";
                }
            } else if (fd == udp_fd) {
                sockaddr_in client_addr{};
                socklen_t addr_len = sizeof(client_addr);
                UdpData data{};

                ssize_t len = recvfrom(udp_fd, &data, sizeof(data), 0,
                        (sockaddr*)&client_addr, &addr_len);
                if (len == sizeof(UdpData)) {
                    // Convert position to host byte order if needed (optional, for cross-endian)
                    // data.id = ntohl(data.id);

                    char client_ip[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
                    std::cout << "[UDP] From " << client_ip << ":" << ntohs(client_addr.sin_port)
                        << " id=" << data.id
                        << " pos=(" << data.pos.x << ", " << data.pos.y << ")\n";

                    // --- Store client address if new ---
                    bool known = false;
                    for (auto &c : udp_clients) {
                        if (c.sin_addr.s_addr == client_addr.sin_addr.s_addr &&
                                c.sin_port == client_addr.sin_port) {
                            known = true;
                            break;
                        }
                    }
                    if (!known) {
                        udp_clients.push_back(client_addr);
                        std::cout << "[UDP] New client tracked (" << client_ip << ")\n";
                    }

                    // --- Broadcast the received data to everyone (including sender for now) ---
                    for (auto &c : udp_clients) {
                        sendto(udp_fd, &data, sizeof(data), 0,
                                (sockaddr*)&c, sizeof(c));
                    }
                }

            } else {
                // --- TCP: existing client ---
                char buffer[1024] = {0};
                ssize_t count = read(fd, buffer, sizeof(buffer) - 1);
                if (count <= 0) {
                    // TODO: Cleanup the stuff when disconnected
                    auto it = std::find(this->pid.begin(), this->pid.end(), fd);
                    if (it != this->pid.end()) {
                        this->pid.erase(it);
                        std::cout << "[TCP] Removed fd " << fd << " from pid list\n";
                    }
                    std::cout << "[TCP] Client disconnected, fd=" << fd << "\n";
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
                    close(fd);
                } else {
                    buffer[count] = '\0';
                    if (strncmp(buffer, "id", 2) == 0) {
                        int id = this->pid.size();
                        this->pid.push_back(fd);
                        int net_id = htonl(id);
                        send(fd, &net_id, sizeof(net_id), 0);
                        std::cout << "[TCP] Assigned id " << id << " to fd " << fd << "\n";
                    }
                }
            }
        }
    }
}
