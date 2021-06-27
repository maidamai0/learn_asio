#include <fmt/core.h>
#include <fmt/ostream.h>

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/placeholders.hpp>
#include <asio/write.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

std::string make_daytime_string() {
  time_t now = time(nullptr);
  return ctime(&now);
}

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
 public:
  using Pointer = std::shared_ptr<TcpConnection>;

  static Pointer Create(asio::io_context& io_ctx) {
    // std::make_shared won't work due to private constructor
    return Pointer{new TcpConnection(io_ctx)};
  }

  static size_t getNumber() { return cnt_; }

  void Start() {
    fmt::print("{} port {} opened\n", __PRETTY_FUNCTION__, socket_.remote_endpoint().port());
    message_ = make_daytime_string();
    message_ += "client address is " + socket_.remote_endpoint().address().to_string() + ":" +
                std::to_string(socket_.remote_endpoint().port()) + "\n\n";
    asio::async_write(
        socket_, asio::buffer(message_),
        std::bind(&TcpConnection::write, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
  }

  asio::ip::tcp::socket& MutableSocket() { return socket_; }
  const asio::ip::tcp::socket& Socket() const { return socket_; }

  /// used by std::shared_ptr
  ~TcpConnection() {
    fmt::print("{} port {} closed\n", __PRETTY_FUNCTION__, socket_.remote_endpoint().port());
    fmt::print("{} left\n\n", --cnt_);
  }

 private:
  TcpConnection(asio::io_context& io_ctx) : socket_{io_ctx} { fmt::print("{}st connection opened\n", ++cnt_); }

  void write(const asio::error_code ec, size_t len) {
    if (ec) {
      throw asio::system_error(ec);
    } else {
      fmt::print("write len:{}\n", len);
    }
  }

 private:
  asio::ip::tcp::socket socket_;
  std::string message_;
  static size_t cnt_;
};

size_t TcpConnection::cnt_ = 0;

class TcpServer {
 public:
  TcpServer(asio::io_context& io_ctx, const int port)
      : io_context_(io_ctx), acceptor_{io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)} {
    fmt::print("{} listen on {}\n", __PRETTY_FUNCTION__, port);
    start_accept();
  }

  ~TcpServer() {
    fmt::print("{} server {}:{} closed\n", __PRETTY_FUNCTION__, acceptor_.local_endpoint().address(),
               acceptor_.local_endpoint().port());
  }

 private:
  void start_accept() {
    auto connection = TcpConnection::Create(io_context_);
    acceptor_.async_accept(connection->MutableSocket(),
                           std::bind(&TcpServer::accept, this, connection, std::placeholders::_1));
  }

  void accept(TcpConnection::Pointer connection, const asio::error_code& ec) {
    if (ec) {
      throw asio::system_error(ec);
    }

    connection->Start();
    start_accept();
  }

 private:
  asio::io_context& io_context_;
  asio::ip::tcp::acceptor acceptor_;
};

int main(int argc, char** argv) {
  if (argc != 2) {
    fmt::print(std::cerr, "Usage daytime3 <port>\n");
    return 1;
  }

  try {
    asio::io_context io_ctx;
    TcpServer server(io_ctx, std::stoi(argv[1]));
    io_ctx.run();

  } catch (const std::exception& e) {
    fmt::print(std::cerr, "{}:{} {}\n", __FILE__, __LINE__, e.what());
  }
}