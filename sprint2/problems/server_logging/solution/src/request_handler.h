#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>
#include <boost/json.hpp>
#include <string_view>

#include "http_server.h"
#include "model.h"
#include "utils.h"
#include "logger.h"

namespace http_handler {
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace json = boost::json;
    namespace logging = boost::log;

    class RequestHandler {
    public:
        explicit RequestHandler(model::Game& game, const std::filesystem::path& static_root)
            : game_{ game }, static_root_{ static_root } {}

        RequestHandler(const RequestHandler&) = delete;
        RequestHandler& operator=(const RequestHandler&) = delete;

        template <typename Body, typename Allocator, typename Send>
        void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send);

    private:
        template <typename Send>
        void HandleStaticRequest(http::request<http::string_body>&& req, Send&& send);

        template <typename Send>
        void HandleApiRequest(http::request<http::string_body>&& req, Send&& send);

        template <typename Send>
        void HandleGetMaps(Send&& send);

        template <typename Send>
        void HandleGetMapById(std::string_view mapId, Send&& send);

        template <typename Send>
        void SendBadRequest(Send&& send);

        template <typename Send>
        void SendNotFound(Send&& send);

        template <typename Send>
        void SendNotFoundPlainText(Send&& send);

        template <typename Send>
        void SendJsonResponse(http::status status, const json::value& jsonValue, Send&& send);

        template <typename Send>
        void SendErrorJsonResponse(http::status status, const std::string& code, const std::string& message, Send&& send);

        json::object SerializeMap(const model::Map& map);

        model::Game& game_;
        std::filesystem::path static_root_;
    };

    template <typename Body, typename Allocator, typename Send>
    void RequestHandler::operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        std::string decodedTarget = utils::UrlDecode(std::string(req.target()));

        if (req.method() == http::verb::get || req.method() == http::verb::head) {
            if (decodedTarget.starts_with("/api/")) {             
                HandleApiRequest(std::forward<decltype(req)>(req), std::forward<Send>(send));
            }
            else {          
                HandleStaticRequest(std::forward<decltype(req)>(req), std::forward<Send>(send));
            }
        }
        else {           
            SendBadRequest(std::forward<Send>(send));
        }
    }

    template <typename Send>
    void RequestHandler::HandleStaticRequest(http::request<http::string_body>&& req, Send&& send) {
        std::string decoded_target = utils::UrlDecode(std::string(req.target()));

        std::filesystem::path file_path = static_root_ / decoded_target.substr(1); // Убираем первый символ '/'
        file_path = file_path.make_preferred(); // Нормализуем путь

        if (std::filesystem::is_directory(file_path)) {
            file_path /= "index.html";
        }

        // Проверка, что файл находится в корневом каталоге
        if (!file_path.string().starts_with(static_root_.string())) {
            SendBadRequest(std::forward<Send>(send));
            return;
        }
        
        if (!std::filesystem::exists(file_path) || !std::filesystem::is_regular_file(file_path)) {            
            SendNotFoundPlainText(std::forward<Send>(send));
            return;
        }
     
        std::ifstream file(file_path, std::ios::binary);
        if (!file) {          
            SendNotFoundPlainText(std::forward<Send>(send));
            return;
        }

        std::stringstream file_content;
        file_content << file.rdbuf();

        
        std::string extension = file_path.extension().string();
        std::string mime_type = utils::GetMimeType(extension);        

        http::response<http::string_body> res{ http::status::ok, req.version() };
        res.set(http::field::content_type, mime_type);
        res.body() = file_content.str();
        res.content_length(res.body().size());
        res.prepare_payload();

        send(std::move(res));
    }

    template <typename Send>
    void RequestHandler::HandleApiRequest(http::request<http::string_body>&& req, Send&& send) {
        std::string decodedTarget = utils::UrlDecode(std::string(req.target()));

        if (decodedTarget == "/api/v1/maps") {
            HandleGetMaps(std::forward<Send>(send));
        }
        else if (decodedTarget.starts_with("/api/v1/maps/")) {
            HandleGetMapById(decodedTarget.substr(12), std::forward<Send>(send));
        }
        else {
            SendBadRequest(std::forward<Send>(send));
        }
    }

    template <typename Send>
    void RequestHandler::HandleGetMaps(Send&& send) {
        json::array mapsJson;
        for (const auto& map : game_.GetMaps()) {
            mapsJson.emplace_back(json::object{
                {"id", *map.GetId()},
                {"name", map.GetName()}
                });
        }

        SendJsonResponse(http::status::ok, mapsJson, std::forward<Send>(send));
    }

    template <typename Send>
    void RequestHandler::HandleGetMapById(std::string_view mapId, Send&& send) {
        std::cout << "Handling request for map: " << mapId << std::endl;

        // Удаляем лишние символы, такие как '/'
        std::string decodedMapId = utils::UrlDecode(std::string(mapId));
        std::string cleanedMapId(decodedMapId);
        cleanedMapId.erase(std::remove(cleanedMapId.begin(), cleanedMapId.end(), '/'), cleanedMapId.end());

        model::Map::Id id{ cleanedMapId };
        std::cout << "Searching for map with id: " << *id << std::endl;

        if (auto mapPtr = game_.FindMap(id); mapPtr) {
            const auto& map = *mapPtr;
            std::cout << "Found map: " << *map.GetId() << " - " << map.GetName() << std::endl;

            SendJsonResponse(http::status::ok, SerializeMap(map), std::forward<Send>(send));
        }
        else {
            std::cout << "Map not found: " << cleanedMapId << std::endl;
            SendNotFound(std::forward<Send>(send));
        }
    }

    template <typename Send>
    void RequestHandler::SendBadRequest(Send&& send) {
        json::object data;
        data["code"] = static_cast<int>(http::status::bad_request);
        data["text"] = "Bad request";
        data["where"] = "request_handler";
        BOOST_LOG_TRIVIAL(error) << logging::add_value(message, "error")       
            << logging::add_value(additional_data, data);
        SendErrorJsonResponse(http::status::bad_request, "badRequest", "Bad request", std::forward<Send>(send));
    }

    template <typename Send>
    void RequestHandler::SendNotFound(Send&& send) {
        json::object data;
        data["code"] = static_cast<int>(http::status::not_found);
        data["text"] = "Map not found";
        data["where"] = "request_handler";
        BOOST_LOG_TRIVIAL(error) << logging::add_value(message, "error")
            << logging::add_value(additional_data, data);

        SendErrorJsonResponse(http::status::not_found, "mapNotFound", "Map not found", std::forward<Send>(send));
    }

    template <typename Send>
    void RequestHandler::SendJsonResponse(http::status status, const json::value& jsonValue, Send&& send) {
        http::response<http::string_body> res{ status, 11 };
        res.set(http::field::content_type, "application/json");
        res.body() = json::serialize(jsonValue);
        res.prepare_payload();
        send(std::move(res));
    }

    template <typename Send>
    void RequestHandler::SendErrorJsonResponse(http::status status, const std::string& code, const std::string& message, Send&& send) {
        json::object errorJson = {
        {"code", code},
        {"message", message}
        };

        http::response<http::string_body> res{ status, 11 };
        res.set(http::field::content_type, "application/json");
        res.body() = json::serialize(errorJson);
        res.prepare_payload();
        send(std::move(res));
    }

    template <typename Send>
    void RequestHandler::SendNotFoundPlainText(Send&& send) {
        json::object data;
        data["code"] = static_cast<int>(http::status::not_found);
        data["text"] = "File not found";
        data["where"] = "request_handler";
        BOOST_LOG_TRIVIAL(error) << logging::add_value(message, "error")
            << logging::add_value(additional_data, data);

        http::response<http::string_body> res{ http::status::not_found, 11 };
        res.set(http::field::content_type, "text/plain");
        res.body() = "File not found";
        res.prepare_payload();
        send(std::move(res));
    }

}  // namespace http_handler