#include "sdk.h"
//
#include <boost/asio/signal_set.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <thread>

#include "json_loader.h"
#include "request_handler.h"

using namespace std::literals;
namespace net = boost::asio;
namespace sys = boost::system;

namespace {

    // Запускает функцию fn на num_threads потоках, включая текущий
    template <typename Fn>
    void RunWorkers(unsigned num_threads, const Fn& fn) {
        num_threads = std::max(1u, num_threads);
        std::vector<std::jthread> workers;
        workers.reserve(num_threads - 1);
        // Запускаем num_threads-1 рабочих потоков, выполняющих функцию fn
        while (--num_threads) {
            workers.emplace_back(fn);
        }
        fn();
    }

}  // namespace

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: game_server <game-config-json> <static-root>"sv << std::endl;
        return EXIT_FAILURE;
    }
    try {
        model::Game game = json_loader::LoadGame(argv[1]);

        const unsigned num_threads = std::thread::hardware_concurrency();
        net::io_context ioc(num_threads);

        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const sys::error_code ec, [[maybe_unused]] int signal_number) {
            if (!ec) {
                std::cout << "Signal "sv << signal_number << " received"sv << std::endl;
                ioc.stop();
            }
            });

        http_handler::RequestHandler handler{ game, argv[2] };

        const auto address = net::ip::make_address("0.0.0.0");
        constexpr net::ip::port_type port = 8081;

        std::cout << "Hello! Server is starting at port " << port << std::endl;
        http_server::ServeHttp(ioc, { address, port }, [&handler](auto&& req, auto&& send) {
            handler(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
            });

        std::cout << "Server has started..."sv << std::endl;

        RunWorkers(std::max(1u, num_threads), [&ioc] {
            ioc.run();
            });
    }
    catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}