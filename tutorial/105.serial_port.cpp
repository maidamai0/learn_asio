#include <iostream>
#include "asio/io_context.hpp"
#include "asio/serial_port.hpp"
#include "asio/write.hpp"

int main() {
  asio::io_context io;
  asio::serial_port port(io, "/dev/ttyS9");
  std::cout << "Open ttyS0 succeed" << std::endl;

  asio::write(port, asio::buffer("Hello, World!\n"));
  io.run();
  return 0;
}