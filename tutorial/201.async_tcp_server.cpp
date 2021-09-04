#include <array>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include "asio/error_code.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"

using socket_t = asio::ip::tcp::socket;
class worker : public std::enable_shared_from_this<worker> {
 public:
  worker(asio::io_context& io_context) : socket_(io_context) {}
  ~worker() { std::cout << "~worker()" << std::endl; }
  void start() {
    std::cout << "Accepted connection from " << socket_.remote_endpoint() << std::endl;
    do_read();
  }
  socket_t& socket() { return socket_; }

 private:
  void do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(asio::buffer(data_), [self](auto ec, auto length) {
      if (!ec) {
        self->do_write(length);
      } else {
        std::cerr << "Error: " << ec.message() << std::endl;
        self->socket_.close();
      }
    });
  }

  void do_write(std::size_t length) {
    std::cout << "Received: " << std::string(data_.data(), length) << std::endl;
  }

 private:
  socket_t socket_;
  std::array<char, 1024> data_;
};
using worker_ptr = std::shared_ptr<worker>;

class server {
 public:
  server(uint16_t port)
      : acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {}

  void run() {
    std::cout << "listening on port " << acceptor_.local_endpoint().port() << std::endl;
    do_accept();
    io_context_.run();
  }

 private:
  void do_accept() {
    worker_ptr new_worker = std::make_shared<worker>(io_context_);
    acceptor_.async_accept(new_worker->socket(), [this, new_worker](const asio::error_code& ec) {
      if (!ec) {
        do_accept();
        new_worker->start();
      } else {
        std::cerr << "Error: " << ec.message() << std::endl;
      }
    });
  }

 private:
  asio::io_context io_context_;
  asio::ip::tcp::acceptor acceptor_;
};

int main(int argc, char** argv) {
  int port = argc > 1 ? atoi(argv[1]) : 6789;
  server s(port);
  s.run();
  return EXIT_SUCCESS;
}