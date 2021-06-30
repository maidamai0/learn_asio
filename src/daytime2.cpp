
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/write.hpp>
#include <ctime>
#include <iostream>
#include <string>

#include "log.hpp"

std::string make_daytime_string() {
  time_t now = time(nullptr);
  return ctime(&now);
}

int main(int argc, char** argv) {
  using namespace asio::ip;
  try {
    if (argc != 2) {
      LOGE("Usage:daytime2 <port>");
      return 1;
    }
    asio::io_context io_ctx;
    asio::ip::tcp::acceptor acceptor(io_ctx, tcp::endpoint(tcp::v4(), std::stoi(argv[1])));

    while (true) {
      tcp::socket socket(io_ctx);
      acceptor.accept(socket);

      std::string message = make_daytime_string();
      asio::error_code ignored_error;
      asio::write(socket, asio::buffer(message), ignored_error);
    }
  } catch (const std::exception& e) {
    LOGE("{}", e.what());
  }

  return 0;
}