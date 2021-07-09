#include "request_parser.h"

#include <array>
#include <cstddef>
#include <string>

#include "common/log.hpp"
#include "http_parser.h"

namespace http {
namespace server {
void request_parser::parse(const char* data, size_t len) {
  http_message::parse(data, len);
}

}  // namespace server
}  // namespace http