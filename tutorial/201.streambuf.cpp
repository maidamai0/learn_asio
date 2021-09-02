#include "asio/buffer.hpp"
#include "asio/connect.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/address.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/streambuf.hpp"

#include <iostream>
#include <ostream>

int main() {
  asio::io_context io_context;
  asio::streambuf streambuf;
  asio::ip::tcp::socket socket(io_context);
  socket.connect(asio::ip::tcp::endpoint(asio::ip::make_address("127.0.0.1"), 5678));

  std::ostream os(&streambuf);
  os << "hello world";
  socket.send(streambuf.data());
  return 0;
}