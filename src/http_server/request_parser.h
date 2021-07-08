#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
namespace http {
namespace server {
class request;
class request_parser {
  using headers_t = std::unordered_map<std::string, std::string>;
  class impl;
  friend class impl;

 public:
  enum class method { GET, POST, PUT, DELETE };
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

  request_parser() = default;
  void reset();
  void parse(const char* data, size_t len);
  std::string print() const;

 private:
  void consume();

 private:
  impl* impl_ = nullptr;

  int http_major = 0;
  int http_minor = 0;
  std::string url_;
};
}  // namespace server
}  // namespace http