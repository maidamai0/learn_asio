#include <fmt/core.h>
#include <fmt/ostream.h>
#include <asio/connect.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>

#include <array>
#include <iostream>
#include <thread>

int main(int argc, char** argv) {
  if (argc != 3) {
    fmt::print(std::cerr, "Usage:daytime1 <host> <port>\n");
    return 1;
  }

  asio::io_context io_ctx;
  asio::ip::tcp::resolver resolver(io_ctx);

  auto endpoints = resolver.resolve(argv[1], argv[2]);

  try {
    asio::ip::tcp::socket socket(io_ctx);
    asio::connect(socket, endpoints);

    fmt::print("local port is {}\n", socket.local_endpoint().port());

    while (true) {
      std::array<char, 128> buf{0};
      asio::error_code ec;

      size_t len = socket.read_some(asio::buffer(buf), ec);
      if (ec == asio::error::eof) {
        // complete
        break;
      } else if (ec) {
        throw asio::system_error(ec);
      }

      std::cout.write(buf.data(), len);
    }
  } catch (const std::exception& e) {
    fmt::print(std::cerr, "{}\n", e.what());
  }

  return 0;
}
