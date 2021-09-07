#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include "asio/buffer.hpp"
#include "asio/error_code.hpp"
#include "asio/ip/basic_endpoint.hpp"
#include "asio/read.hpp"
#include "asio/read_until.hpp"
#include "asio/write.hpp"

using socket_t = asio::ip::tcp::socket;
using port_t = asio::ip::port_type;
using message_handler_t = std::function<void(const std::string&)>;
using error_handler_t = std::function<void()>;

class session : public std::enable_shared_from_this<session> {
  using char_t = char;
  using buffer_t = std::array<char_t, 1024>;
  using string_t = std::basic_string<char_t>;

 public:
  session(asio::io_context& io_context) : socket_(io_context), local_id_(global_id_++) {}
  ~session() = default;

  void start() { chat(); }
  socket_t& socket() { return socket_; }

 private:
  void chat() {
    socket_.async_read_some(
        asio::buffer(buffer_),
        [self = shared_from_this()](const asio::error_code& error, std::size_t bytes_transferred) {
          if (error) {
            std::cout << self->name() << " error: " << error.message() << std::endl;
          } else {
            self->on_message(string_t(self->buffer_.data(), bytes_transferred));
          }
        });
  }

  void on_message(const string_t& message) {
    std::cout << name() << " message received: " << message << std::endl;
    send_message(message);
  }

  void send_message(const string_t& message) {
    std::cout << name() << " sending message: " << message << std::endl;
    asio::async_write(
        socket_, asio::buffer(message),
        [self = shared_from_this()](const asio::error_code& error, std::size_t bytes_transferred) {
          (void)bytes_transferred;
          if (error) {
            std::cerr << self->name() << " error: " << error.message() << std::endl;
            self->socket_.close();
          } else {
            self->chat();
            std::cout << self->name() << " message sent" << std::endl;
          }
        });
  }

  std::string name() const {
    std::stringstream ss;
    ss << "[" << socket_.local_endpoint() << "]";
    return ss.str();
  }

 private:
  socket_t socket_;
  std::array<char_t, 1024> buffer_;
  int64_t local_id_ = 0;
  static int64_t global_id_;
};
int64_t session::global_id_ = 0;
using session_ptr = std::shared_ptr<session>;

class server {
 public:
  server(port_t port)
      : acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {}

 public:
  void run() {
    std::cout << "Listen on " << acceptor_.local_endpoint().port() << std::endl;
    do_accept();
    io_context_.run();
  }

 private:
  void do_accept() {
    session_ptr new_session(new session(io_context_));
    acceptor_.async_accept(new_session->socket(), [this, new_session](std::error_code ec) {
      if (!ec) {
        new_session->start();
        do_accept();
      }
    });
  }

  void broadcast(const std::string& message) {
    for (auto& session : sessions_) {
      asio::async_write(session->socket(), asio::buffer(message),
                        [session](const asio::error_code& error, size_t bytes_transferred) {
                          (void)bytes_transferred;
                          if (error) {
                            std::cerr << "error: " << error.message() << std::endl;
                          } else {
                            std::cout << "message sent" << std::endl;
                          }
                        });
    }
  }

 private:
  asio::io_context io_context_;
  asio::ip::tcp::acceptor acceptor_;
  std::vector<session_ptr> sessions_;
};

int main() {
  server s(5678);
  s.run();
  return 0;
}