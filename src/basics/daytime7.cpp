#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/ip/udp.hpp>
#include <asio/write.hpp>
#include <functional>
#include <iostream>
#include <string>

#include "common/log.hpp"

using asio::ip::tcp;
using asio::ip::udp;

std::string make_daytime_string() {
  time_t now = time(nullptr);
  return ctime(&now);
}

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
 public:
  using connection_t = std::shared_ptr<TcpConnection>;

  static connection_t Create(asio::io_context& io_ctx) { return connection_t{new TcpConnection(io_ctx)}; }

  ~TcpConnection() {}

  void Start() {
    message_ = make_daytime_string();
    asio::async_write(
        socket_, asio::buffer(message_),
        std::bind(&TcpConnection::write, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
  }

  tcp::socket& GetMutableSocket() { return socket_; }

 private:
  TcpConnection(asio::io_context& io_ctx) : socket_{io_ctx} {}
  void write(const asio::error_code& error, size_t len) {
    if (error) {
      LOGE("write error:{}", error.message());
      throw asio::system_error(error);
    }

    LOGI("write len:{}", len);
  }

 private:
  asio::ip::tcp::socket socket_;
  std::string message_;
};

class TcpServer {
 public:
  TcpServer(asio::io_context& io_ctx, unsigned short port)
      : io_context_(io_ctx), acceptor_{io_context_, tcp::endpoint{tcp::v4(), port}} {
    start_accept();
  }

 private:
  void start_accept() {
    auto connection{TcpConnection::Create(io_context_)};
    acceptor_.async_accept(connection->GetMutableSocket(),
                           std::bind(&TcpServer::accept, this, connection, std::placeholders::_1));
  }

  void accept(TcpConnection::connection_t connection, const asio::error_code& ec) {
    if (!ec) {
      connection->Start();
      start_accept();
    } else {
      throw asio::system_error(ec);
    }
  }

 private:
  asio::io_context& io_context_;
  asio::ip::tcp::acceptor acceptor_;
};

class UdpServer {
 public:
  UdpServer(asio::io_context& io_ctx, unsigned short port) : socket_{io_ctx, udp::endpoint{udp::v4(), port}} {
    start_receive();
  }

 private:
  void start_receive() {
    socket_.async_receive_from(asio::buffer(recv_buf_), remote_endpoint_,
                               std::bind(&UdpServer::receive, this, std::placeholders::_1));
  }

  void receive(const asio::error_code& ec) {
    if (!ec) {
      LOGI("receve message[{}] from {}:{}", recv_buf_.data(), remote_endpoint_.address().to_string(),
           remote_endpoint_.port());
      message_ = make_daytime_string();
      socket_.async_send_to(asio::buffer(message_), remote_endpoint_,
                            std::bind(&UdpServer::write, this, std::placeholders::_1, std::placeholders::_2));
    } else {
      LOGE("receive got error:{}", ec.message());
    }
  }

  void write(const asio::error_code& ec, size_t len) {
    if (!ec) {
      LOGI("write len : {}", len);
    } else {
      LOGE("write got error:{}", ec.message());
    }
  }

 private:
  udp::socket socket_;
  std::string message_;
  udp::endpoint remote_endpoint_;
  std::array<char, 1> recv_buf_;
};

int main(int argc, char** argv) {
  if (argc != 3) {
    LOGE("Usage: daytime7 <tcp port> <udp port>");
    return 1;
  }

  try {
    asio::io_context io_ctx;
    TcpServer tcp_server(io_ctx, static_cast<unsigned short>(std::stoi(argv[1])));
    UdpServer udp_server(io_ctx, static_cast<unsigned short>(std::stoi(argv[2])));

    io_ctx.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}