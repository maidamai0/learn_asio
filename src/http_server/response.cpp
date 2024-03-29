
#include "response.h"

#include <ctime>
#include <sstream>
#include <string>
#include <iomanip>

namespace http::server {

namespace status_strings {
const std::string ok = "HTTP/1.0 200 OK\r\n";
const std::string created = "HTTP/1.0 201 Created\r\n";
const std::string accepted = "HTTP/1.0 202 Accepted\r\n";
const std::string no_content = "HTTP/1.0 204 No Content\r\n";
const std::string multiple_choices = "HTTP/1.0 300 Multiple Choices\r\n";
const std::string moved_permanently = "HTTP/1.0 301 Moved Permanently\r\n";
const std::string moved_temporarily = "HTTP/1.0 302 Moved Temporarily\r\n";
const std::string not_modified = "HTTP/1.0 304 Not Modified\r\n";
const std::string bad_request = "HTTP/1.0 400 Bad Request\r\n";
const std::string unauthorized = "HTTP/1.0 401 Unauthorized\r\n";
const std::string forbidden = "HTTP/1.0 403 Forbidden\r\n";
const std::string not_found = "HTTP/1.0 404 Not Found\r\n";
const std::string internal_server_error = "HTTP/1.0 500 Internal Server Error\r\n";
const std::string not_implemented = "HTTP/1.0 501 Not Implemented\r\n";
const std::string bad_gateway = "HTTP/1.0 502 Bad Gateway\r\n";
const std::string service_unavailable = "HTTP/1.0 503 Service Unavailable\r\n";

asio::const_buffer to_buffer(response::status_code status) {
  switch (status) {
    case response::status_code::ok:
      return asio::buffer(ok);
    case response::status_code::created:
      return asio::buffer(created);
    case response::status_code::accepted:
      return asio::buffer(accepted);
    case response::status_code::no_content:
      return asio::buffer(no_content);
    case response::status_code::multiple_choices:
      return asio::buffer(multiple_choices);
    case response::status_code::moved_permanently:
      return asio::buffer(moved_permanently);
    case response::status_code::moved_temporarily:
      return asio::buffer(moved_temporarily);
    case response::status_code::not_modified:
      return asio::buffer(not_modified);
    case response::status_code::bad_request:
      return asio::buffer(bad_request);
    case response::status_code::unauthorized:
      return asio::buffer(unauthorized);
    case response::status_code::forbidden:
      return asio::buffer(forbidden);
    case response::status_code::not_found:
      return asio::buffer(not_found);
    case response::status_code::internal_server_error:
      return asio::buffer(internal_server_error);
    case response::status_code::not_implemented:
      return asio::buffer(not_implemented);
    case response::status_code::bad_gateway:
      return asio::buffer(bad_gateway);
    case response::status_code::service_unavailable:
      return asio::buffer(service_unavailable);
    default:
      return asio::buffer(internal_server_error);
  }
}

}  // namespace status_strings

namespace misc_strings {

const char name_value_separator[] = {':', ' '};
const char crlf[] = {'\r', '\n'};

}  // namespace misc_strings

std::vector<asio::const_buffer> response::to_buffers() {
  std::vector<asio::const_buffer> buffers;
  buffers.push_back(status_strings::to_buffer(status));

  std::time_t now;
  std::time(&now);
  std::stringstream ss;
  tm t;
#ifdef _WIN32
  gmtime_s(&t, &now);
#else
  gmtime_r(&now, &t);
#endif
  ss << std::put_time(&t, "%a, %d %b %Y %T GMT");
  headers_[kDate] = ss.str();
  headers_[kContentLength] = std::to_string(content.size());

  for (const auto& header : headers_) {
    buffers.push_back(asio::buffer(header.first));
    buffers.push_back(asio::buffer(misc_strings::name_value_separator));
    buffers.push_back(asio::buffer(header.second));
    buffers.push_back(asio::buffer(misc_strings::crlf));
  }
  buffers.push_back(asio::buffer(misc_strings::crlf));
  buffers.push_back(asio::buffer(content));
  return buffers;
}

namespace stock_replies {

const char ok[] = "";
const char created[] =
    "<html>"
    "<head><title>Created</title></head>"
    "<body><h1>201 Created</h1></body>"
    "</html>";
const char accepted[] =
    "<html>"
    "<head><title>Accepted</title></head>"
    "<body><h1>202 Accepted</h1></body>"
    "</html>";
const char no_content[] =
    "<html>"
    "<head><title>No Content</title></head>"
    "<body><h1>204 Content</h1></body>"
    "</html>";
const char multiple_choices[] =
    "<html>"
    "<head><title>Multiple Choices</title></head>"
    "<body><h1>300 Multiple Choices</h1></body>"
    "</html>";
const char moved_permanently[] =
    "<html>"
    "<head><title>Moved Permanently</title></head>"
    "<body><h1>301 Moved Permanently</h1></body>"
    "</html>";
const char moved_temporarily[] =
    "<html>"
    "<head><title>Moved Temporarily</title></head>"
    "<body><h1>302 Moved Temporarily</h1></body>"
    "</html>";
const char not_modified[] =
    "<html>"
    "<head><title>Not Modified</title></head>"
    "<body><h1>304 Not Modified</h1></body>"
    "</html>";
const char bad_request[] =
    "<html>"
    "<head><title>Bad Request</title></head>"
    "<body><h1>400 Bad Request</h1></body>"
    "</html>";
const char unauthorized[] =
    "<html>"
    "<head><title>Unauthorized</title></head>"
    "<body><h1>401 Unauthorized</h1></body>"
    "</html>";
const char forbidden[] =
    "<html>"
    "<head><title>Forbidden</title></head>"
    "<body><h1>403 Forbidden</h1></body>"
    "</html>";
const char not_found[] =
    "<html>"
    "<head><title>Not Found</title></head>"
    "<body><h1>404 Not Found</h1></body>"
    "</html>";
const char internal_server_error[] =
    "<html>"
    "<head><title>Internal Server Error</title></head>"
    "<body><h1>500 Internal Server Error</h1></body>"
    "</html>";
const char not_implemented[] =
    "<html>"
    "<head><title>Not Implemented</title></head>"
    "<body><h1>501 Not Implemented</h1></body>"
    "</html>";
const char bad_gateway[] =
    "<html>"
    "<head><title>Bad Gateway</title></head>"
    "<body><h1>502 Bad Gateway</h1></body>"
    "</html>";
const char service_unavailable[] =
    "<html>"
    "<head><title>Service Unavailable</title></head>"
    "<body><h1>503 Service Unavailable</h1></body>"
    "</html>";

std::string to_string(response::status_code status) {
  switch (status) {
    case response::status_code::ok:
      return ok;
    case response::status_code::created:
      return created;
    case response::status_code::accepted:
      return accepted;
    case response::status_code::no_content:
      return no_content;
    case response::status_code::multiple_choices:
      return multiple_choices;
    case response::status_code::moved_permanently:
      return moved_permanently;
    case response::status_code::moved_temporarily:
      return moved_temporarily;
    case response::status_code::not_modified:
      return not_modified;
    case response::status_code::bad_request:
      return bad_request;
    case response::status_code::unauthorized:
      return unauthorized;
    case response::status_code::forbidden:
      return forbidden;
    case response::status_code::not_found:
      return not_found;
    case response::status_code::internal_server_error:
      return internal_server_error;
    case response::status_code::not_implemented:
      return not_implemented;
    case response::status_code::bad_gateway:
      return bad_gateway;
    case response::status_code::service_unavailable:
      return service_unavailable;
    default:
      return internal_server_error;
  }
}

}  // namespace stock_replies

response::response(const status_code code) {
  status = code;
  content = stock_replies::to_string(status);
  headers_[kContentLength] = std::to_string(content.size());
}

response::response() {
  status = status_code::ok;
  headers_[kContentType] = "text/json";
  headers_[kServer] = "tonghao&asio";
  headers_[kConnection] = "close";
}

}  // namespace http::server
