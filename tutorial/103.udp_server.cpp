#include <cstdint>
#include <iostream>
#include "asio/io_context.hpp"
#include "asio/ip/udp.hpp"

int main() {
  uint16_t port = 5678;
  asio::io_context io;
  asio::ip::udp::socket socket(io);
  asio::ip::udp::endpoint receiver(asio::ip::udp::v4(), port);
  std::cout << "Listening on port " << port << std::endl;
  socket.open(asio::ip::udp::v4());
  while (true) {
    asio::ip::udp::endpoint sender;
    std::array<char, 1024> buffer;
    asio::error_code error;
    const auto size = socket.receive_from(asio::buffer(buffer), sender);
    std::cout << "Received " << size << " bytes from " << sender.address() << std::endl;
    socket.send_to(asio::buffer(buffer, size), sender);
  }
  return 0;
}