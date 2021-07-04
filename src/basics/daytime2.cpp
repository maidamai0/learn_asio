
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/write.hpp>
#include <ctime>
#include <iostream>
#include <string>

#include "common/log.hpp"

std::string make_daytime_string() {
  time_t now = time(nullptr);
  return ctime(&now);
}

int main() {
  using namespace asio::ip;
  try {
    int port = 13;
    asio::io_context io_ctx;
    asio::ip::tcp::acceptor acceptor(io_ctx, tcp::endpoint(tcp::v4(), 13));
    LOGI("daytime server is running on {}", port);

    while (true) {
      tcp::socket socket(io_ctx);
      acceptor.accept(socket);
      LOGI("accept remote {}:{}", socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port());

      std::string message = make_daytime_string();
      asio::error_code ignored_error;
      asio::write(socket, asio::buffer(message), ignored_error);
    }
  } catch (const std::exception& e) {
    LOGE("{}", e.what());
  }

  return 0;
}