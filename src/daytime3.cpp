#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/placeholders.hpp>
#include <asio/write.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "log.hpp"

using namespace std::chrono_literals;

std::string make_daytime_string() {
  time_t now = time(nullptr);
  return ctime(&now);
}

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
 public:
  using connection_t = std::shared_ptr<TcpConnection>;

  static connection_t Create(asio::io_context& io_ctx) {
    // std::make_shared won't work due to private constructor
    return connection_t{new TcpConnection(io_ctx)};
  }

  void Start() {
    LOGI("port {} opened", socket_.remote_endpoint().port());
    message_ = make_daytime_string();
    message_ += "client address is " + socket_.remote_endpoint().address().to_string() + ":" +
                std::to_string(socket_.remote_endpoint().port()) + '\n';
    asio::async_write(
        socket_, asio::buffer(message_),
        std::bind(&TcpConnection::write, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
  }

  asio::ip::tcp::socket& MutableSocket() { return socket_; }
  const asio::ip::tcp::socket& Socket() const { return socket_; }

  /// used by std::shared_ptr
  ~TcpConnection() { LOGI("port {} closed", socket_.remote_endpoint().port()); }

 private:
  TcpConnection(asio::io_context& io_ctx) : socket_{io_ctx} {}

  void write(const asio::error_code ec, size_t len) {
    std::this_thread::sleep_for(5s);
    if (ec) {
      throw asio::system_error(ec);
    } else {
      LOGI("write len:{}", len);
    }
  }

 private:
  asio::ip::tcp::socket socket_;
  std::string message_;
};

class TcpServer {
 public:
  TcpServer(asio::io_context& io_ctx, const int port)
      : io_context_(io_ctx), acceptor_{io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)} {
    LOGI("daytime server running on {}:{}", acceptor_.local_endpoint().address().to_string(), port);
    start_accept();
  }

  ~TcpServer() {
    LOGI("server {}:{} closed", acceptor_.local_endpoint().address().to_string(), acceptor_.local_endpoint().port());
  }

 private:
  void start_accept() {
    LOGI("waiting for client...");
    auto connection = TcpConnection::Create(io_context_);
    acceptor_.async_accept(connection->MutableSocket(),
                           std::bind(&TcpServer::accept, this, connection, std::placeholders::_1));
  }

  void accept(TcpConnection::connection_t connection, const asio::error_code& ec) {
    LOGI("client accepted");
    if (ec) {
      LOGW("error on accept:{}", ec.message());
      throw asio::system_error(ec);
    }

    connection->Start();
    start_accept();
  }

 private:
  asio::io_context& io_context_;
  asio::ip::tcp::acceptor acceptor_;
};

int main() {
  try {
    asio::io_context io_ctx;
    int port = 13;
    TcpServer server(io_ctx, port);
    io_ctx.run();

  } catch (const std::exception& e) {
    LOGE("{}:{} {}", __FILE__, __LINE__, e.what());
  }
}