#include "client.h"
#include "data.h"
#include "game.h"

#include <arpa/inet.h>
#include <unistd.h>

Client::~Client() { close(this->sockudp); }
bool Client::connect_to_server() {
    // UDP
    sockudp = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockudp < 0) {
        perror("socket failed");
        return false;
    }

    sockaddr_in server_addr_udp{};
    server_addr_udp.sin_family = AF_INET;
    server_addr_udp.sin_port = htons(this->port);
    inet_pton(AF_INET, this->ip, &server_addr_udp.sin_addr);

    if (connect(sockudp, (sockaddr*)&server_addr_udp, sizeof(server_addr_udp)) < 0) {
        perror("connect failed");
        close(sockudp);
        return false;
    }

    // TCP
    socktcp = socket(AF_INET, SOCK_STREAM, 0);
    if (socktcp < 0) {
        perror("socket failed");
        return false;
    }

    sockaddr_in server_addr_tcp{};
    server_addr_tcp.sin_family = AF_INET;
    server_addr_tcp.sin_port = htons(this->port);
    inet_pton(AF_INET, this->ip, &server_addr_tcp.sin_addr);

    if (connect(socktcp, (sockaddr*)&server_addr_tcp, sizeof(server_addr_tcp)) < 0) {
        perror("connect failed");
        close(socktcp);
        return false;
    }

    return true;
}
void Client::loop() {
    Game *game = (Game *)this->game;
    for (;;) {
        if (game->exit) break;

        UdpData msg = {0};
        if (game->p1) {
            msg.pos = { game->p1->rec.x, game->p1->rec.y };
            if (send(sockudp, (void *)&msg, sizeof(msg), 0) < 0) {
                perror("send failed");
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

    close(sockudp);
}
