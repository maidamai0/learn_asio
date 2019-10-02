#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <asio/signal_set.hpp>

#include <array>
#include <functional>
#include <string>
#include <iostream>

using asio::ip::udp;

std::string make_daytime_string() {
  time_t now = time(nullptr);
  return ctime(&now);
}

class UdpServer{
public:
UdpServer(asio::io_context& io_ctx, const int port):socket_{io_ctx, udp::v4(), port}{}

private:
 void start_receive(){
     socket_.async_receive_from(asio::buffer(recv_buffer_), remote_endpoint_, std::bind(&UdpServer::receive, this));
 }

 void receive(const asio::error_code &ec, size_t len) {
     if(!ec){
         std::string message{make_daytime_string()};
         socket_.async_send_to(asio::buffer(message), remote_endpoint_, std::bind(&UdpServer::send, this, message, std::placeholders::_1, std::placeholders::_2));
         start_receive();
     }
 }

 void send(std::string && message, const asio::error_code &ec, size_t len){

 }

private:
udp::socket socket_;
udp::endpoint remote_endpoint_;
std::array<char, 1> recv_buffer_{0}; // what happens if cliend send more data?
};

void hander(asio::io_context &io_ctx, const asio::error_code &ec, int signal_number){
    switch (signal_number)
    {
        case SIGSEGV:
        case SIGINT:
        case SIGTERM: {
            fmt::print("got signal {}, exit...\n", signal_number);
            io_ctx.stop();
            break;
        }
        default:
            break;
    }
}

int main (int argc, char **argv) {
    if(argc != 2) {
        fmt::print(std::cerr, "Usage: daytime6 <port>\n");
        return 1;
    }

    asio::io_context io_ctx;
    UdpServer server(io_ctx, std::stoi(argv[1]));

    asio::signal_set signals(io_ctx, SIGSEGV, SIGINT, SIGTERM);
    signals.async_wait(std::bind(std::ref(io_ctx), std::placeholders::_1, std::placeholders::_2));

    io_ctx.run();

    return 0;
}