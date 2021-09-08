#include <array>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <iostream>
#include <sstream>
#include <string>

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include "asio/buffer.hpp"
#include "asio/error_code.hpp"
#include "asio/ip/basic_endpoint.hpp"
#include "asio/read.hpp"
#include "asio/read_until.hpp"
#include "asio/streambuf.hpp"
#include "asio/write.hpp"

using socket_t = asio::ip::tcp::socket;
using port_t = asio::ip::port_type;
using message_handler_t = std::function<void(const std::string&)>;

class session : public std::enable_shared_from_this<session> {
 public:
  session(asio::io_context& io_context, message_handler_t handler)
      : socket_(io_context), message_handler_(handler) {}
  ~session() {
    if (message_handler_) {
      message_handler_(name_ + " has left the chat");
    }
  };

  void start() {
    std::stringstream ss;
    ss << "[" << socket_.remote_endpoint() << "]: ";
    name_ = ss.str();
    async_read();
  }
  socket_t& socket() { return socket_; }

  void post(const std::string& msg) {
    const auto write_on_idle = outgoing_messages_.empty();
    outgoing_messages_.emplace_back(std::move(msg));
    if (write_on_idle) {
      async_write();
    }
  }

 private:
  void async_read() {
    asio::async_read_until(
        socket_, buffer_, '\n',
        [self = shared_from_this()](const asio::error_code& ec, std::size_t bytes_transferred) {
          self->on_read(ec, bytes_transferred);
        });
  }

  void async_write() {
    asio::async_write(
        socket_, asio::buffer(outgoing_messages_.front()),
        [self = shared_from_this()](const asio::error_code& ec, std::size_t bytes_transferred) {
          self->on_write(ec, bytes_transferred);
        });
  }

  void on_read(const asio::error_code& error, std::size_t bytes_transferred) {
    if (error) {
      return on_error(error);
    }

    std::string line;
    std::istream is(&buffer_);
    std::getline(is, line);
    buffer_.consume(bytes_transferred);
    std::cout << name_ << line << std::endl;

    if (message_handler_) {
      message_handler_(name_ + line);
    }

    post(std::move(line));
  }

  void on_write(const asio::error_code& error, std::size_t bytes_transferred) {
    (void)bytes_transferred;
    if (error) {
      return on_error(error);
    }

    outgoing_messages_.pop_front();
    if (!outgoing_messages_.empty()) {
      async_write();
    }
  }

  void on_error(const asio::error_code& error) {
    std::cerr << name_ << "error, " << error.message() << std::endl;
    socket_.close();
  }

 private:
  socket_t socket_;
  asio::streambuf buffer_;
  std::deque<std::string> outgoing_messages_;
  message_handler_t message_handler_;
  std::string name_;
};

using session_ptr = std::shared_ptr<session>;

class server {
 public:
  server(port_t port)
      : acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {}

 public:
  void run() {
    std::cout << "Listen on " << acceptor_.local_endpoint().port() << std::endl;
    accept();
    io_context_.run();
  }

 private:
  void accept() {
    session_ptr new_session(
        new session(io_context_, std::bind(&server::broadcast, this, std::placeholders::_1)));
    acceptor_.async_accept(new_session->socket(),
                           [this, new_session](std::error_code ec) { on_accept(ec, new_session); });
  }

  void on_accept(const asio::error_code& ec, session_ptr new_session) {
    if (ec) {
      std::cerr << "error, " << ec.message() << std::endl;
      return;
    }
    std::stringstream ss;
    ss << new_session->socket().remote_endpoint() << " joined\n";
    broadcast(ss.str());

    new_session->post("Welcome to the chat server\n");
    new_session->start();

    sessions_.push_back(new_session);

    accept();
  }

  void broadcast(const std::string& message) {
    for (auto& session : sessions_) {
      session->post(message);
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