#include <iostream>
#include "asio/io_context.hpp"
#include "asio/ip/udp.hpp"

int main() {
  asio::io_context io;
  asio::ip::udp::socket socket(io);
  asio::ip::udp::endpoint endpoint(asio::ip::udp::v4(), 5678);
  socket.open(endpoint.protocol());
  while (true) {
    std::array<char, 128> buf;
    std::cin >> buf.data();
    socket.send_to(asio::buffer(buf.data(), buf.size()), endpoint);
    socket.receive_from(asio::buffer(buf.data(), buf.size()), endpoint);
    std::cout << buf.data() << std::endl;
  }
  return 0;
}