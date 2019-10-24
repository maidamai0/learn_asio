#include <fmt/core.h>
#include <fmt/ostream.h>
#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <asio/signal_set.hpp>

#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

using asio::ip::udp;

std::string make_daytime_string() {
  time_t now = time(nullptr);
  return ctime(&now);
}

class UdpServer {
 public:
  UdpServer(asio::io_context& io_ctx, const int port)
      : socket_{io_ctx, udp::endpoint(udp::v4(), port)} {
    start_receive();
  }

 private:
  void start_receive() {
    fmt::print("Listen on {}:{}\n",
               socket_.local_endpoint().address().to_string(),
               socket_.local_endpoint().port());
    socket_.async_receive_from(
        asio::buffer(recv_buffer_), remote_endpoint_,
        std::bind(&UdpServer::receive, this, std::placeholders::_1,
                  std::placeholders::_2));
  }

  void receive(const asio::error_code& ec, size_t len) {
    if (len != recv_buffer_.size()) {
      fmt::print("Warning: receive len[{}] is not equal to buffer size[{}]\n",
                 len, recv_buffer_.size());
    }

    fmt::print("remote is {}:{}\n", remote_endpoint_.address().to_string(),
               remote_endpoint_.port());

    std::shared_ptr<std::string> message;
    if (!ec) {
      message = std::make_shared<std::string>(make_daytime_string());
    } else {
      fmt::print(std::cerr, "receive got error:{}\n", ec);
      message = std::make_shared<std::string>(ec.message());
    }

    socket_.async_send_to(
        asio::buffer(*message), remote_endpoint_,
        std::bind(&UdpServer::send, this, message, std::placeholders::_1,
                  std::placeholders::_2));
    start_receive();
  }

  void send(std::shared_ptr<std::string> message, const asio::error_code& ec,
            size_t len) {
    if (ec) {
      fmt::print(std::cerr, "Send message {} error:{}, len:{}\n", *message, ec,
                 len);
    }
  }

 private:
  udp::socket socket_;
  udp::endpoint remote_endpoint_;
  std::array<char, 1> recv_buffer_{0};
};

void handler(asio::io_context& io_ctx, const asio::error_code&,
             int signal_number) {
  switch (signal_number) {
  case SIGSEGV:
  case SIGINT:
  case SIGTERM: {
    fmt::print("got signal {}, exit...\n", signal_number);
    io_ctx.stop();
    break;
  }
  default:
    break;
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fmt::print(std::cerr, "Usage: daytime6 <port>\n");
    return 1;
  }

  asio::io_context io_ctx;
  UdpServer server(io_ctx, std::stoi(argv[1]));

  asio::signal_set signals(io_ctx, SIGSEGV, SIGINT, SIGTERM);
  signals.async_wait(std::bind(handler, std::ref(io_ctx), std::placeholders::_1,
                               std::placeholders::_2));

  io_ctx.run();

  return 0;
}