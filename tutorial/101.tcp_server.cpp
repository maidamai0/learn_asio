#include <cstdint>
#include <cstdlib>
#include <iostream>

#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/write.hpp"
#include "cxxopts.hpp"

int main(int argc, char* argv[]) {
  int port = 5678;
  if (argc > 1) {
    port = std::atoi(argv[1]);
  }

  try {
    asio::io_context io_context;
    asio::ip::tcp::acceptor acceptor(io_context,
                                     asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
    std::cout << "server running on " << port << '\n';
    while (true) {
      asio::ip::tcp::socket socket(io_context);
      acceptor.accept(socket);
      std::cout << "accepted connection: " << socket.remote_endpoint() << '\n';
      asio::write(socket, asio::buffer("Hello, world!\n"));
    }
  } catch (const std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}