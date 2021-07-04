#pragma once
#include <string_view>

#include "asio/error_code.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/signal_set.hpp"
#include "common/macro.hpp"
#include "connection_manager.h"

namespace http {
namespace server {
class server {
 public:
  no_copy(server);
  server(const int port, std::string_view root);
  void run();

 private:
  void start_accept();
  void hanlde_accept(const asio::error_code& ec);
  void hanlde_stop(const asio::error_code& ec, int sig);

 private:
  asio::io_context io_context_;
  asio::signal_set signals_;
  asio::ip::tcp::acceptor acceptor_;
  connection_manager connection_manager_;
  connection::pointer new_connection_;
  request_handler request_handler_;
};
}  // namespace server
}  // namespace http