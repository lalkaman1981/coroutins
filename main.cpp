#include "myfp.h"
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

void send_all(int fd, const std::string &msg) {
  send(fd, msg.c_str(), msg.size(), 0);
}

int main() {
  mycomanager manager;

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("socket");
    return 1;
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8080);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(server_fd, (sockaddr *)&addr, sizeof(addr)) != 0) {
    perror("bind");
    return 1;
  }
  if (listen(server_fd, 16) != 0) {
    perror("listen");
    return 1;
  }

  std::cout << "Server running at http://localhost:8080\n";

  // ================== ACCEPT LOOP ==================
  myco_async(manager, [&manager, server_fd]() {
    while (true) {
      int client_fd = accept(server_fd, nullptr, nullptr);
      if (client_fd < 0) {
        perror("accept");
        continue;
      }

      std::cout << "[server] client connected\n";

      // Spawn handler coroutine — PASS MANAGER EXPLICITLY
      myco_async(manager, [client_fd]() {
        char buffer[1024];

        int n = read(client_fd, buffer, sizeof(buffer) - 1);
        if (n <= 0) {
          close(client_fd);
          return;
        }
        buffer[n] = 0;

        std::string req = buffer;
        std::cout << "[handler] got request:\n" << req << "\n";

        const std::string response = "HTTP/1.1 200 OK\r\n"
                                     "Content-Type: text/plain\r\n"
                                     "Content-Length: 12\r\n"
                                     "Connection: close\r\n"
                                     "\r\n"
                                     "Hello World";

        send_all(client_fd, response);
        close(client_fd);

        std::cout << "[handler] done\n";
      });

      // yield accept-loop coroutine
      mycotask::current_task()->yield();
    }
  });

  manager.run();
  return 0;
}
