#include "http_server.h"

namespace http_server {

// Разместите здесь реализацию http-сервера, взяв её из задания по разработке асинхронного сервера

}  // namespace http_server
#include "http_server.h"

#include <boost/asio/dispatch.hpp>
#include <iostream>



namespace http_server {


    void SessionBase::Run() {
        // Вызываем метод Read, используя executor объекта stream_.
        // Таким образом вся работа со stream_ будет выполняться, используя его executor
        net::dispatch(stream_.get_executor(),
            beast::bind_front_handler(&SessionBase::Read, GetSharedThis()));
    }

}  // namespace http_server