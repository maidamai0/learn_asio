#include "connection_manager.h"

namespace http {
namespace server {
void connection_manager::start(connection::pointer con) {
  connections_.insert(con);
  con->start();
}

void connection_manager::stop(connection::pointer con) {
  con->stop();
  connections_.erase(con);
}

void connection_manager::stop_all() {
  for (auto con : connections_) {
    con->stop();
  }
  connections_.clear();
}

}  // namespace server
}  // namespace http