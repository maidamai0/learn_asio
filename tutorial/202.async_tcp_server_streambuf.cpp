#include "asio/error.hpp"
#include "asio/error_code.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/basic_endpoint.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/post.hpp"
#include "asio/read_until.hpp"
#include "asio/streambuf.hpp"
#include "asio/write.hpp"

#include <iostream>

using socket_t = asio::ip::tcp::socket;
using buffer_t = asio::streambuf;
using port_t = asio::ip::port_type;

class session : public std::enable_shared_from_this<session> {
 public:
  session(asio::io_context& io_context) : socket_(io_context) {}
  socket_t& socket() { return socket_; }
  void start() { read(); }

 private:
  void read() {
    asio::async_read_until(socket_, buffer_, '\n',
                           [self = shared_from_this()](auto ec, auto bytes_transferred) {
                             self->on_read(ec, bytes_transferred);
                           });
  }

  void write(const std::string& message) {
    asio::async_write(
        socket_, asio::buffer(message),
        [self = shared_from_this()](const asio::error_code& ec, size_t bytes_transferred) {
          self->on_write(ec, bytes_transferred);
        });
  }

  void on_read(const asio::error_code& ec, std::size_t bytes_transferred) {
    if (ec) {
      return on_error(ec);
    }

    std::string line;
    std::istream is(&buffer_);
    std::getline(is, line);
    write(std::move(line));
    buffer_.consume(bytes_transferred);
    read();
  }

  void on_write(const asio::error_code& ec, std::size_t bytes_transferred) {
    if (ec) {
      return on_error(ec);
    }

    (void)bytes_transferred;
  }

  void on_error(const asio::error_code& ec) {
    socket_.close();
    std::cerr << "Error: " << ec.message() << ", closed" << std::endl;
  }

 private:
  socket_t socket_;
  buffer_t buffer_;
};
using session_ptr = std::shared_ptr<session>;

class server {
 public:
  server(port_t port)
      : port_(port), acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port_)) {}

  void run() { accept(); }

 private:
  void accept() {
    auto new_session = std::make_shared<session>(io_context_);
    acceptor_.async_accept(
        new_session->socket(), [new_session, this](const asio::error_code& error) {
          if (error) {
            std::cout << "Error: " << error.message() << std::endl;
          } else {
            std::cout << "Accepted " << new_session->socket().remote_endpoint() << std::endl;
            new_session->start();
            accept();
          }
        });
  }

 private:
  asio::io_context io_context_;
  const port_t port_ = 6789;
  asio::ip::tcp::acceptor acceptor_;
};

int main() {
  return 0;
}