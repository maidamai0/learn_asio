
#include <array>
#include <asio/connect.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <thread>

#include "common/log.hpp"
#include "cxxopts.hpp"

int main(int argc, char** argv) {
  std::string host;
  try {
    cxxopts::Options options("daytime1", "get date time");
    // clang-format off
    options.add_options()
    ("h,host", "host name or ip address", cxxopts::value<std::string>(host)->implicit_value("localhost"));
    // clang-format on
    const auto args = options.parse(argc, argv);
  } catch (cxxopts::OptionException& e) {
    LOGE("{}", e.what());
    return EXIT_FAILURE;
  }

  asio::io_context io_ctx;
  asio::ip::tcp::resolver resolver(io_ctx);

  auto results = resolver.resolve(host, "daytime");

  LOGI("endpoints list:");
  for (const auto& result : results) {
    LOGI("{} => {}:{}", result.host_name(), result.endpoint().address().to_string(), result.endpoint().port());
  }

  try {
    asio::ip::tcp::socket socket(io_ctx);
    asio::connect(socket, results);

    LOGI("local port is {}", socket.local_endpoint().port());

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
    LOGE("{}", e.what());
  }

  return 0;
}
