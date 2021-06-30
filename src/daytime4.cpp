#include <array>
#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <iostream>

#include "log.hpp"

using asio::ip::udp;

int main(int argc, char** argv) {
  if (argc != 3) {
    LOGE("Usage daytime4 <host> <port>");
    return 1;
  }

  try {
    asio::io_context io_ctx;
    udp::resolver resolver(io_ctx);
    udp::endpoint remote_endpoint = *resolver.resolve(udp::v4(), argv[1], argv[2]).begin();

    udp::socket socket(io_ctx);
    socket.open(udp::v4());
    std::array<char, 1> send_buf{0};
    socket.send_to(asio::buffer(send_buf), remote_endpoint);

    LOGI("remote endpoint is {}:{}", remote_endpoint.address().to_string(), remote_endpoint.port());

    std::array<char, 128> recv_buf{0};
    udp::endpoint sender_endpoint;
    size_t len = socket.receive_from(asio::buffer(recv_buf), sender_endpoint);
    LOGI("local endpoint is {}:{}", socket.local_endpoint().address().to_string(), socket.local_endpoint().port());

    assert(remote_endpoint.address() == sender_endpoint.address());
    assert(remote_endpoint.port() == sender_endpoint.port());

    std::cout.write(recv_buf.data(), len);
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
}