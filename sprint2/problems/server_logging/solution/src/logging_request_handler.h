#pragma once
//#include "request_handler.h"
//#include "logger.h"
#include <boost/json.hpp>
#include <chrono>

namespace logging = boost::log;

namespace http_handler {

    template<class SomeRequestHandler>
    class LoggingRequestHandler {
    public:
        LoggingRequestHandler(SomeRequestHandler& decorated)
            : decorated_(decorated) {}

        template <typename Body, typename Allocator, typename Send>
        void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
            auto start_time = std::chrono::steady_clock::now();
            LogRequest(req);

            decorated_(std::move(req), [this, start_time, send = std::forward<Send>(send)](auto&& response) {
                LogResponse(response, start_time);
                send(std::move(response));
                });
        }

    private:
        void LogRequest(const http::request<http::string_body>& req) {
            json::object data;
            data["ip"] = std::string(req.base().at(http::field::host).data(), req.base().at(http::field::host).size());
            data["URI"] = std::string(req.target().data(), req.target().size());
            data["method"] = std::string(req.method_string().data(), req.method_string().size());

            BOOST_LOG_TRIVIAL(info) << logging::add_value(message, "request received")
                << logging::add_value(additional_data, data);
        }

        void LogResponse(const http::response<http::string_body>& resp, std::chrono::steady_clock::time_point start_time) {
            auto end_time = std::chrono::steady_clock::now();
            auto response_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

            json::object data;
            data["response_time"] = response_time;
            data["code"] = resp.result_int();
            if (resp.find(http::field::content_type) != resp.end()) {
                data["content_type"] = resp[http::field::content_type].data();
            }
            else {
                data["content_type"] = nullptr;
            }

            BOOST_LOG_TRIVIAL(info) << logging::add_value(message, "response sent")
                << logging::add_value(additional_data, data);
        }

        SomeRequestHandler& decorated_;
    };

}  // namespace http_handler