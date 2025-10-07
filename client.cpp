#include "client.h"
#include "data.h"
#include "game.h"

#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>

void udp_thread_func(const char* server_ip, int server_port, void *g) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket failed");
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    if (connect(sockfd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect failed");
        close(sockfd);
        return;
    }

    std::cout << "UDP connected to " << server_ip << ":" << server_port << std::endl;

    Game *game = (Game *)g;
    for (;;) {
        if (game->exit) break;

        UdpData msg = {0};
        if (game->p1) {
            msg.pos = { game->p1->rec.x, game->p1->rec.y };
            if (send(sockfd, (void *)&msg, sizeof(msg), 0) < 0) {
                perror("send failed");
            } else {
                std::cout << "the data are: " << msg.pos.x << ", " << msg.pos.y << std::endl;
            }
        }
        usleep(200000);
    }

    // char buf[1024];
    // ssize_t len = recv(sockfd, buf, sizeof(buf) - 1, 0);
    // if (len > 0) {
    //     buf[len] = '\0';
    //     std::cout << "Received: " << buf << std::endl;
    // } else if (len == 0) {
    //     std::cout << "No data received" << std::endl;
    // } else {
    //     perror("recv failed");
    // }

    close(sockfd);
}
