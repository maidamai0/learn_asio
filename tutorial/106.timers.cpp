#include <iostream>
#include "asio/error_code.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/address.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/steady_timer.hpp"

int main() {
  asio::io_context io;
  asio::ip::tcp::socket socket(io);

  asio::steady_timer timer(io, asio::chrono::seconds(1));
  timer.async_wait([&](const asio::error_code& ec) {
    if (!ec) {
      std::cout << "Timer expired\n";
      socket.close();
    }
  });

  socket.async_connect(asio::ip::tcp::endpoint(asio::ip::make_address("127.0.0.1"), 9876),
                       [&timer](const asio::error_code& ec) {
                         if (!ec) {
                           timer.cancel();
                           std::cout << "Connected!" << std::endl;
                         }
                       });

  io.run();

  return 0;
}