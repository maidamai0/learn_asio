#pragma once

/**
 * @file message.h
 * @author tonghao.yuan (yuantonghao@gmail.com)
 * @brief  base class for  response and request, representing a message
 * @version 0.1
 * @date 2021-07-09
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace http {
namespace server {
class request;

enum class message_status { complete, want_more, faild };
class http_message {
  using headers_t = std::unordered_map<std::string, std::string>;
  class impl;
  friend class impl;

 public:
  enum class method { GET, POST, PUT, DELETE, INVALID };

  static constexpr auto kHeader = "Host";
  static constexpr auto kAcceptEncoding = "Accept-Encoding";
  static constexpr auto kAcceptLanguage = "Accept-Language";
  static constexpr auto kUserAgent = "User-Agent";
  static constexpr auto KAccept = "Accept";
  static constexpr auto kConnection = "Connection";
  static constexpr auto KTransferEncoding = "Transfer-Encoding";

  static constexpr auto kDate = "Date";
  static constexpr auto kServer = "Server";
  static constexpr auto KLastModified = "Last-Modified";
  static constexpr auto kETag = "ETag";
  static constexpr auto kAcceptRanges = "Accept-Ranges";
  static constexpr auto kContentLength = "Content-Length";
  static constexpr auto kContentType = "Content-Type";

  http_message();
  ~http_message();
  void reset();
  message_status parse(const char* data, size_t len);

 private:
  void print();
  void save_headers();

 public:
  method method_ = method::INVALID;
  int http_major_ = 0;
  int http_minor_ = 0;
  std::string url_;
  headers_t headers_;
  std::string body_;
  int status_ = 0;
  bool upgrade_ = false;  // websocket?
  bool keep_alive_ = false;

 private:
  std::unique_ptr<impl> pimpl_;
  std::string current_header_field_;
  std::string current_header_value_;
  message_status message_status_ = message_status::want_more;
};
}  // namespace server
}  // namespace http