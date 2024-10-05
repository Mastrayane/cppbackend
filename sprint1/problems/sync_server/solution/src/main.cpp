#ifdef WIN32
#include <sdkddkver.h>
#endif
// boost.beast будет использовать std::string_view вместо boost::string_view
#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <thread>
#include <optional>

#include <chrono>
using namespace std::chrono;

namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std::literals;
namespace beast = boost::beast;
namespace http = beast::http;

int main() {
    // Выведите строчку "Server has started...", когда сервер будет готов принимать подключения

    // Контекст для выполнения операций ввода-вывода.
   // Другие классы библиотеки выполняют с его помощью операции ввода-вывода
    net::io_context io;

    // Замеряем время начала программы
    const auto start_time = steady_clock::now();

    {
        auto t = std::make_shared<net::steady_timer>(io, 5s);
        std::cout << "Brew coffee"sv << std::endl;
        t->async_wait([t](boost::system::error_code ec) {
            if (ec) {
                throw std::runtime_error("Wait error: "s + ec.message());
            }
            std::cout << "Pour coffee in the cup. Thread id: "sv << std::this_thread::get_id()
                << std::endl;
            });
    }

    {
        auto t = std::make_shared<net::steady_timer>(io, 3s);
        std::cout << "Fry eggs"sv << std::endl;
        // Захват переменной t внутри лямбда-функции продлит время жизни таймера
        // до тех пор, пока не будет вызван обработчик.
        t->async_wait([t](boost::system::error_code ec) {
            if (ec) {
                throw std::runtime_error("Wait error: "s + ec.message());
            }
            std::cout << "Put eggs onto the plate. Thread id: "sv << std::this_thread::get_id()
                << std::endl;
            });
    }

    try {
        std::cout << "Run asynchronous operations"sv << std::endl;
        io.run();
        const auto cook_duration = duration<double>(steady_clock::now() - start_time);
        std::cout << "Breakfast has been cooked in "sv << cook_duration.count() << "s"sv << std::endl;
        std::cout << "Thread id: "sv << std::this_thread::get_id() << std::endl;
        std::cout << "Enjoy your meal"sv << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}
