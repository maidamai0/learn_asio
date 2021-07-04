#pragma once

#include <set>

#include "common/macro.hpp"
#include "connection.h"

namespace http {
namespace server {
class connection_manager {
 public:
  connection_manager() = default;
  no_copy(connection_manager);
  void start(connection::pointer con);
  void stop(connection::pointer con);
  void stop_all();

 private:
  std::set<connection::pointer> connections_;
};
}  // namespace server
}  // namespace http