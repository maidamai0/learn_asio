#include "asio/buffer.hpp"
#include "asio/connect.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/address.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/streambuf.hpp"

#include <cstring>
#include <iostream>
#include <istream>
#include <ostream>

int main() {
  asio::streambuf streambuf;
  {
    auto view = streambuf.prepare(64);
    std::memcpy(view.data(), "Hello, world!", 13);
    streambuf.commit(13);
    std::cout << std::istream(&streambuf).rdbuf() << std::endl;
    streambuf.consume(13);
  }

  {
    std::ostream os(&streambuf);
    os << "hello world";
    streambuf.commit(os.rdbuf()->in_avail());
    std::cout << std::istream(&streambuf).rdbuf() << std::endl;
    streambuf.consume(os.rdbuf()->in_avail());
  }

  return 0;
}