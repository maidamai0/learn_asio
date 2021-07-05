#include <cstdlib>
#include <istream>
#include <ostream>
#include <string>
#include <thread>

#include "asio/connect.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/read_until.hpp"
#include "asio/streambuf.hpp"
#include "asio/write.hpp"
#include "common/log.hpp"
#include "cxxopts.hpp"

using namespace std::chrono_literals;

int main(int argc, char** argv) {
  try {
    std::string port = "80";
    cxxopts::Options options(APP_NAME, "A HTTP client");
    // clang-format off
    options.add_options()
    ("p, port", "tcp port", cxxopts::value(port)->default_value("80"));
    // clang-format on
    options.parse(argc, argv);

    asio::io_context io_context;

    asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("localhost", port);

    while (true) {
      asio::ip::tcp::socket socket(io_context);
      asio::connect(socket, endpoints);
      LOGI("local port is {}", socket.local_endpoint().port());

      std::this_thread::sleep_for(1s);
      asio::streambuf request;
      std::ostream request_stream(&request);
      request_stream << "GET /index.html"
                     << " HTTP/1.0\r\n";
      request_stream << "Host: localhost\r\n";
      request_stream << "Accept: */*\r\n";
      request_stream << "Connection: close\r\n\r\n";

      asio::write(socket, request);

      asio::streambuf response;
      asio::read_until(socket, response, "\r\n");
      std::istream response_stream(&response);
      std::string http_version;
      response_stream >> http_version;
      int status_code;
      response_stream >> status_code;
      std::string status_message;
      std::getline(response_stream, status_message);
      if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
        LOGW("invalid response");
        continue;
      }

      if (status_code != 200) {
        LOGW("response not ok:{}", status_code);
        continue;
      }

      asio::read_until(socket, response, "\r\n\r\n");
      std::string header;
      while (std::getline(response_stream, header) && header != "\r") {
        LOGI("{}", header);
      }
    }

  } catch (cxxopts::OptionException& e) {
    LOGE("{}", e.what());
    return EXIT_FAILURE;
  }

  return 0;
}