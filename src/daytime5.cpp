#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <ctime>
#include <iostream>
#include <string>

#include "log.hpp"

using asio::ip::udp;

std::string make_daytime_string() {
  time_t now = time(nullptr);
  return ctime(&now);
}

int main(int argc, char** argv) {
  if (argc != 2) {
    LOGE("Usage daytime5 <port>\n");
    return 1;
  }

  try {
    asio::io_context io_ctx;
    udp::socket socket(io_ctx, udp::endpoint(udp::v4(), std::stoi(argv[1])));

    while (true) {
      std::array<char, 1> rev_buf{0};
      udp::endpoint remote_endpoint;
      asio::error_code ec;

      socket.receive_from(asio::buffer(rev_buf), remote_endpoint);

      std::string message(make_daytime_string());
      asio::error_code ignored_eror;
      socket.send_to(asio::buffer(message), remote_endpoint, 0, ignored_eror);
    }

  } catch (const std::exception& e) {
    LOGE("Failed: {}", e.what());
  }
}