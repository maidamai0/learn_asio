#pragma once

#include <string>
#include <vector>

#include "header.hpp"

namespace http {
namespace server {
struct request {
  std::string method;
  std::string url;
  std::string http_version_major;
  std::string http_version_minor;
  std::vector<header> headers;
};
}  // namespace server
}  // namespace http