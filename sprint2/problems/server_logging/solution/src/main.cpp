#include "sdk.h"
#include <boost/asio/signal_set.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <thread>

#include "json_loader.h"
#include "request_handler.h"
#include "logging_request_handler.h"
//#include "logger.h"

using namespace std::literals;
namespace net = boost::asio;
namespace sys = boost::system;
namespace logging = boost::log;
namespace json = boost::json;

namespace {

    template <typename Fn>
    void RunWorkers(unsigned num_threads, const Fn& fn) {
        num_threads = std::max(1u, num_threads);
        std::vector<std::jthread> workers;
        workers.reserve(num_threads - 1);
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
        init_logging();

        model::Game game = json_loader::LoadGame(argv[1]);

        const unsigned num_threads = std::thread::hardware_concurrency();
        net::io_context ioc(num_threads);

        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const sys::error_code ec, [[maybe_unused]] int signal_number) {
            if (!ec) {
                json::object data;
                data["code"] = 0;
                BOOST_LOG_TRIVIAL(info) << logging::add_value(message, "server exited")
                    << logging::add_value(additional_data, data);
                ioc.stop();
            }
            });

        http_handler::RequestHandler handler{ game, argv[2] };
        http_handler::LoggingRequestHandler logging_handler{ handler };

        const auto address = net::ip::make_address("0.0.0.0");
        constexpr net::ip::port_type port = 8080;

        json::object data;
        data["port"] = port;
        data["address"] = address.to_string();
        BOOST_LOG_TRIVIAL(info) << logging::add_value(message, "server started")
            << logging::add_value(additional_data, data);

        std::cout << "Hello! Server is starting at port " << port << std::endl;
        http_server::ServeHttp(ioc, { address, port }, [&logging_handler](auto&& req, auto&& send) {
            logging_handler(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
            });

        std::cout << "Server has started..."sv << std::endl;

        RunWorkers(std::max(1u, num_threads), [&ioc] {
            ioc.run();
            });
    }
    catch (const std::exception& ex) {
        json::object data;
        data["code"] = EXIT_FAILURE;
        data["exception"] = ex.what();
        BOOST_LOG_TRIVIAL(info) << logging::add_value(message, "server exited")
            << logging::add_value(additional_data, data);
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}