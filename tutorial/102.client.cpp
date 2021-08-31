#include <cstdlib>
#include <iostream>

#include "asio/connect.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"

int main(int argc, char** argv) {
  int port = 5678;
  if (argc > 1) {
    port = atoi(argv[1]);
  }

  asio::io_context io_context;
  asio::ip::tcp::resolver resolver(io_context);
  asio::ip::tcp::resolver::results_type endpoints =
      resolver.resolve("localhost", std::to_string(port));

  asio::ip::tcp::socket socket(io_context);
  asio::connect(socket, endpoints);

  std::vector<char> buffer(1024);
  socket.read_some(asio::buffer(buffer));
  std::cout << "Received: " << buffer.data() << std::endl;

  return 0;
}