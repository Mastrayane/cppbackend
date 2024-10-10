#include "audio.h"
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

using namespace std::literals;
namespace net = boost::asio;
using net::ip::udp;

void StartServer(uint16_t port) {
    Recorder recorder(ma_format_u8, 1);
    Player player(ma_format_u8, 1);

    net::io_context io_context;
    udp::socket socket(io_context, udp::endpoint(udp::v4(), port));

    while (true) {
        std::array<char, 65000> recv_buf;
        udp::endpoint remote_endpoint;

        auto size = socket.receive_from(net::buffer(recv_buf), remote_endpoint);
        std::cout << "Received data from " << remote_endpoint << std::endl;

        // Вычисляем количество фреймов в сообщении
        size_t frame_size = player.GetFrameSize();
        size_t frames = size / frame_size;

        player.PlayBuffer(recv_buf.data(), frames, 1.5s);
        std::cout << "Playing done" << std::endl;
    }
}

void StartClient(uint16_t port) {
    Recorder recorder(ma_format_u8, 1);

    net::io_context io_context;
    udp::socket socket(io_context, udp::endpoint(udp::v4(), port));

    while (true) {
        std::string server_ip;
        std::cout << "Enter server IP: ";
        std::getline(std::cin, server_ip);

        udp::endpoint server_endpoint(net::ip::make_address(server_ip), port);

        auto rec_result = recorder.Record(65000, 1.5s);
        std::cout << "Recording done, sending data..." << std::endl;

        // Вычисляем количество байт, которые нужно передать
        size_t frame_size = recorder.GetFrameSize();
        size_t bytes_to_send = rec_result.frames * frame_size;

        boost::system::error_code ec;
        socket.send_to(net::buffer(rec_result.data.data(), bytes_to_send), server_endpoint, 0, ec);

        if (ec) {
            std::cerr << "Error sending data: " << ec.message() << std::endl;
        }
        else {
            std::cout << "Data sent successfully" << std::endl;
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <client|server> <port>" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    int port = std::stoi(argv[2]);

    if (mode == "client") {
        StartClient(port);
    }
    else if (mode == "server") {
        StartServer(port);
    }
    else {
        std::cerr << "Invalid mode. Use 'client' or 'server'." << std::endl;
        return 1;
    }

    return 0;
}