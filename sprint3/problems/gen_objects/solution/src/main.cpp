#include <boost/asio/signal_set.hpp>
#include <boost/asio/io_context.hpp>

#include <iostream>
#include <thread>

#include "request_handler.h"
#include "ticker.h"
#include "command_line_parser.h"
#include "logger.h"

using namespace std::literals;
namespace net = boost::asio;
namespace sys = boost::system;
namespace fs = std::filesystem;

namespace {

    template <typename Fn>
    void RunWorkers(unsigned n, const Fn& fn) {
        n = std::max(1u, n);
        std::vector<std::jthread> workers;
        workers.reserve(n - 1);
        while (--n) {
            workers.emplace_back(fn);
        }
        fn();
    }

} // namespace

int main(int argc, const char* argv[]) {

    Args command_line_args;
    try {
        if (auto args = ParseCommandLine(argc, argv)) {
            command_line_args = *args;
        }
        else {
            return EXIT_FAILURE;
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Failed parsing command line arguments: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    logger::Logger logger;
    logger.Init();

    try {
        // Логирование аргументов командной строки

        /*
        boost::json::object cmd_args_log;
        cmd_args_log["config_file_path"] = command_line_args.config_file_path;
        cmd_args_log["static_root"] = command_line_args.static_root;
        cmd_args_log["tick_period"] = command_line_args.tick_period;
        cmd_args_log["random_spawn"] = command_line_args.random_spawn;
        logger::LogMessageInfo(cmd_args_log, "Parsed command line arguments");
        */
        

        fs::path config = fs::weakly_canonical(fs::path(auxillary::UrlDecode(command_line_args.config_file_path)));
        fs::path root = fs::weakly_canonical(fs::path(auxillary::UrlDecode(command_line_args.static_root)));

        // Логирование путей к конфигурационному файлу и статическим ресурсам

        /*
         boost::json::object paths_log;
        paths_log["config_path"] = config.string();
        paths_log["static_root_path"] = root.string();
        logger::LogMessageInfo(paths_log, "Resolved paths");
        */
       

        const unsigned num_threads = std::thread::hardware_concurrency();
        net::io_context ioc(num_threads);

        auto api_strand = net::make_strand(ioc);

        GameServer gs(ioc, config, root);

        if (command_line_args.random_spawn == true) {
            gs.SetSpawnDogRandomPoint();
        }

        if (command_line_args.tick_period > 0) {
            std::chrono::milliseconds mills(command_line_args.tick_period);
            auto ticker = std::make_shared<Ticker>(api_strand, mills,
                [&gs](std::chrono::milliseconds delta) {gs.Tick(delta); }
            );
            gs.SetAutoTicker();
            ticker->Start();
        }

        const auto address = net::ip::make_address("0.0.0.0");
        constexpr net::ip::port_type port = 8080;

        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc, &logger](const sys::error_code ec, [[maybe_unused]] int signal_number) {
            if (!ec) {
                ioc.stop();
            }
        });

        auto handler = std::make_shared<http_handler::RequestHandler>(ioc, gs, api_strand);
        http_handler::LoggingRequestHandler<http_handler::RequestHandler> logging_handler{ *handler };
        boost::json::object add_data;
        add_data["port"] = port;
        add_data["address"] = address.to_string();
        logger::LogMessageInfo(add_data, "server started"s);
        // Запускаем обработку запросов 
        http_server::ServeHttp(ioc, { address, port }, logging_handler);

        RunWorkers(std::max(1u, num_threads), [&ioc] {
            ioc.run();
        });
    }
    catch (const std::exception& ex) {
        logger::LogExit(EXIT_FAILURE, &ex);
        return EXIT_FAILURE;
    }
    logger::LogExit(0);
    return 0;
}