#include "request.h"

#include <array>
#include <cstddef>
#include <string>

#include "common/log.hpp"
#include "http_parser.h"

namespace http {
namespace server {
message_status request::parse(const char* data, size_t len) {
  return http_message::parse(data, len);
}

}  // namespace server
}  // namespace http