#include <iostream>

#include "asio/error_code.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"

int main() {
  asio::io_context io_context;
  asio::ip::tcp::resolver resolver(io_context);
  asio::error_code error;
  for (const auto& entry : resolver.resolve("www.google.com", "80", error)) {
    std::cout << entry.endpoint() << std::endl;
  }
}