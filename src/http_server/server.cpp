#include "server.h"

#include <functional>
#include <memory>

#include "asio/ip/tcp.hpp"
#include "common/log.hpp"

namespace http {
namespace server {
server::server(const int port, std::string_view root)
    : signals_(io_context_, SIGINT, SIGTERM),
      acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
      request_handler_(root) {
  signals_.async_wait(
      std::bind(&server::hanlde_stop, this, std::placeholders::_1, std::placeholders::_2));
  start_accept();
}

void server::run() {
  io_context_.run();
}

void server::start_accept() {
  new_connection_ =
      std::make_shared<connection>(io_context_, connection_manager_, request_handler_);
  acceptor_.async_accept(new_connection_->socket(),
                         std::bind(&server::hanlde_accept, this, std::placeholders::_1));
}

void server::hanlde_accept(const asio::error_code& ec) {
  if (!acceptor_.is_open()) {
    LOGW("server is closed");
    return;
  }

  if (ec) {
    LOGW("accepted a connection with error: {}", ec.message());
  } else {
    LOGI("new client:{}:{}", new_connection_->socket().remote_endpoint().address().to_string(),
         new_connection_->socket().remote_endpoint().port());
    connection_manager_.start(new_connection_);
  }

  start_accept();
}

// FIXME (tonghao): 2021-07-09
// not called
void server::hanlde_stop(const asio::error_code& ec, int sig) {
  if (ec) {
    LOGE("{}", ec.message());
  }

  switch (sig) {
    case SIGINT:
      LOGW("SIGINT received");
      break;
    case SIGTERM:
      LOGW("SIGTERM received");
      break;
  }

  acceptor_.close();
  connection_manager_.stop_all();
}

}  // namespace server
}  // namespace http