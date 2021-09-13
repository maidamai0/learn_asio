#include "asio/buffer.hpp"
#include "asio/connect.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/read.hpp"
#include "asio/read_until.hpp"
#include "asio/streambuf.hpp"
#include "asio/write.hpp"

#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>

class application {
 public:
  application(asio::io_context& io_context, const std::string& host)
      : resolver_(io_context), socket_(io_context) {
    std::cout << host << std::endl;
    std::stringstream ss;
    ss << "GET / HTTP/1.1\n"
       << "Host: " << host << '\n'
       << "Connection: close\n\n";
    request_ = ss.str();
    resolver_.async_resolve(
        host, "http",
        std::bind(&application::on_resolve, this, std::placeholders::_1, std::placeholders::_2));
  }

 private:
  void on_resolve(const asio::error_code& ec, asio::ip::tcp::resolver::results_type results) {
    if (ec) {
      return on_error(ec);
    }

    std::cout << "Addresses: \n";
    for (const auto& res : results) {
      std::cout << "  " << res.endpoint() << std::endl;
    }

    asio::async_connect(
        socket_, results,
        std::bind(&application::on_connect, this, std::placeholders::_1, std::placeholders::_2));
  }

  void on_connect(const asio::error_code& ec, const asio::ip::tcp::endpoint& endpoint) {
    if (ec) {
      return on_error(ec);
    }
    std::cout << "Connected to: " << endpoint << std::endl;

    std::cout << request_ << std::endl;

    asio::async_write(
        socket_, asio::buffer(request_),
        std::bind(&application::on_write, this, std::placeholders::_1, std::placeholders::_2));
  }
  void on_write(const asio::error_code& ec, std::size_t bytes_transferred) {
    (void)bytes_transferred;
    if (ec) {
      return on_error(ec);
    }

    std::cout << "Request sent: " << bytes_transferred << " bytes" << std::endl;
    asio::async_read(
        socket_, response_,
        std::bind(&application::on_read, this, std::placeholders::_1, std::placeholders::_2));
  }

  void on_read(const asio::error_code& ec, std::size_t bytes_transferred) {
    if (bytes_transferred > 0) {
      std::cout << std::istream(&response_).rdbuf() << std::endl;
      response_.consume(bytes_transferred);
    }
    if (ec) {
      on_error(ec);
    } else {
      asio::async_read(
          socket_, response_,
          std::bind(&application::on_read, this, std::placeholders::_1, std::placeholders::_2));
    }
  }

  void on_error(const asio::error_code& ec) {
    std::cerr << "Error: " << ec.message() << std::endl;
    if (socket_.is_open()) {
      socket_.close();
    }
  }

 private:
  asio::ip::tcp::resolver resolver_;
  asio::ip::tcp::socket socket_;
  std::string request_;
  asio::streambuf response_;
};

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: http_client <host>" << std::endl;
  }
  asio::io_context io_context;
  application app(io_context, argv[1]);
  io_context.run();
  return 0;
}