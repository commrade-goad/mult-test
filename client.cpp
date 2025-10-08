#include "client.h"
#include "data.h"
#include "game.h"
#include "id.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>

Client::~Client() {
    close(this->sockudp);
    close(this->socktcp);
}

bool Client::connect_to_server() {
    // --- UDP ---
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

    // make UDP non-blocking
    int flags = fcntl(sockudp, F_GETFL, 0);
    fcntl(sockudp, F_SETFL, flags | O_NONBLOCK);

    // --- TCP ---
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

    // --- Request ID ---
    int id;
    const char *msg = "id";
    Game *game = (Game *)this->game;
    if (send(socktcp, msg, strlen(msg), 0) < 0) {
        perror("send failed");
    }

    ssize_t len = recv(socktcp, &id, sizeof(id), 0);
    if (len == sizeof(id)) game->p1->id = ntohl(id);

    return true;
}

void Client::loop() {
    Game *game = (Game *)this->game;

    for (;;) {
        if (game->exit) break;

        // --- Send position ---
        if (game->p1) {
            UdpData msg{};
            msg.id = game->p1->id;
            msg.pos = { game->p1->rec.x, game->p1->rec.y };
            if (send(sockudp, &msg, sizeof(msg), 0) < 0) {
                perror("UDP send failed");
            }
        }

        // --- Receive updates ---
        for (;;) {
            UdpData recv_msg{};
            ssize_t len = recv(sockudp, &recv_msg, sizeof(recv_msg), 0);
            if (len <= 0) break;

            if (len == sizeof(UdpData)) {
                if (recv_msg.id == game->p1->id) continue;

                std::shared_ptr<Player> found = nullptr;
                for (auto &o : game->obs) {
                    auto p = std::dynamic_pointer_cast<Player>(o);
                    if (p && p->id == recv_msg.id) {
                        found = p;
                        break;
                    }
                }

                if (!found) {
                    auto pptr = std::make_shared<Player>(PLAYER);
                    pptr->col = GetColor(0x1e1e2eff);
                    pptr->id = recv_msg.id;
                    pptr->rec = { recv_msg.pos.x, recv_msg.pos.y,
                        static_cast<float>(game->tile_size), static_cast<float>(game->tile_size) };
                    game->obs.push_back(pptr);
                    std::cout << "[UDP] New player " << recv_msg.id
                        << " created at (" << recv_msg.pos.x << ", " << recv_msg.pos.y << ")\n";
                } else {
                    // found->rec.x = recv_msg.pos.x;
                    // found->rec.y = recv_msg.pos.y;

                    found->rec.x += (recv_msg.pos.x - found->rec.x) * 0.2f;
                    found->rec.y += (recv_msg.pos.y - found->rec.y) * 0.2f;
                }

                std::cout << "[UDP] id=" << recv_msg.id
                          << " pos=(" << recv_msg.pos.x << ", " << recv_msg.pos.y << ")\n";
            }
        }

        usleep(16000);
    }

    close(sockudp);
    close(socktcp);
}
