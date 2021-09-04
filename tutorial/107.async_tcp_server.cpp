#include <array>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>

#include "asio/error_code.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"

using socket_t = asio::ip::tcp::socket;

class client {
 public:
  using client_ptr = std::shared_ptr<client>;

  static client_ptr create(asio::io_context& io_context) {
    return client_ptr(new client(io_context));
  }

  socket_t& socket() { return socket_; }

 private:
  client(asio::io_context& io_context) : socket_(io_context) {}

 private:
  socket_t socket_;
};

void handle_accept(client::client_ptr client, asio::error_code ec) {
  if (!ec) {
    std::cout << "accepted: " << client->socket().remote_endpoint() << std::endl;
  }
}

int main(int argc, char** argv) {
  int port = argc > 1 ? atoi(argv[1]) : 6789;
  asio::io_context io_context;
  asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
  std::cout << "listening on port " << port << std::endl;

  auto client = client::create(io_context);
  acceptor.async_accept(client->socket(), [client](const asio::error_code& ec) {
    if (ec) {
      std::cerr << "accept failed: " << ec.message() << std::endl;
      return;
    }
    std::cout << "handle_accept: " << client->socket().remote_endpoint() << std::endl;
  });
  io_context.run();

  return 0;
}