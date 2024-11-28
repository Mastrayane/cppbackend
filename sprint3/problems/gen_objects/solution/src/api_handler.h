#pragma once

#include <algorithm>
#include <cctype>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <optional>

#include <string>
#include <boost/beast/http/status.hpp>

#include "auxiliary.h"
#include "game_server.h"
#include "response_maker.h"
#include "tagged.h"
#include "request_handler.h"

namespace json = boost::json;
namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

namespace http_handler {


    std::string statusClassToString(boost::beast::http::status_class status_class) {
        switch (status_class) {
        case boost::beast::http::status_class::informational: return "Informational";
        case boost::beast::http::status_class::successful: return "Successful";
        case boost::beast::http::status_class::redirection: return "Redirection";
        case boost::beast::http::status_class::client_error: return "Client Error";
        case boost::beast::http::status_class::server_error: return "Server Error";
        default: return "Unknown";
        }
    }

    std::string ToString(RequestType type) {
        switch (type) {
        case RequestType::API: return "API";
        case RequestType::PLAYER: return "PLAYER";
        case RequestType::FILE: return "FILE";
        default: return "UNKNOWN";
        }
    }



boost::json::value PrepareRoadsForResponse(std::shared_ptr<model::Map> map);
boost::json::value PrepareBuildingsForResponse(std::shared_ptr<model::Map> map);
boost::json::value PrepareOfficesForResponse(std::shared_ptr<model::Map> map);
boost::json::value PrepareMapForResponse(std::shared_ptr<model::Map> map);
boost::json::value PrepareLootTypesForResponse(std::shared_ptr<model::Map> map);

class ApiHandler {
public:
    explicit ApiHandler(GameServer& gs) :
        gs_(gs) {
        }

    ~ApiHandler() {
    }

    template <typename Body, typename Allocator>
    http::response<http::string_body> HandleRequest(const http::request<Body, http::basic_fields<Allocator>>& req_, const RequestData& r_data_) {
        try {
            if (r_data_.type == RequestType::API) {
                return HandleMapRequest(req_, r_data_);
            }
            if (r_data_.type == RequestType::PLAYER) {
                if (r_data_.r_target == "players") {
                    return HandlePlayersListRequest(req_);
                } else if (r_data_.r_target == "join") {
                    return HandlePlayerJoinRequest(req_);
                } else if (r_data_.r_target == "state") {
                    return HandleStateRequest(req_);
                } else if (r_data_.r_target == "action") {
                    return HandleActionRequest(req_);
                } else if (r_data_.r_target == "tick" && !gs_.IsAutoTicker()) {
                    return HandleTickRequest(req_);
                } else {

                    // ����������� ������
                    std::cout << "Invalid request target: " << r_data_.r_target << std::endl;

                    return MakeResponse(http::status::bad_request, 
                                        Errors::BAD_REQ, 
                                        req_.version(), req_.keep_alive(), 
                                        ContentType::JSON);
                }
            } 
        } catch (const std::exception& ex) {

            // ����������� ������
            std::cout << "Exception: " << ex.what() << std::endl;

            return MakeResponse(http::status::bad_request, 
                                Errors::BAD_REQ, 
                                req_.version(), req_.keep_alive(), 
                                ContentType::JSON);
        }

        // ����������� ������
        std::cout << "Unknown request type: " << ToString(r_data_.type) << std::endl;

        return MakeResponse(http::status::bad_request, 
                            Errors::BAD_REQ, 
                            req_.version(), req_.keep_alive(), 
                            ContentType::JSON);
    }

// Methods, no authorization required ->
    template <typename Body, typename Allocator>
    http::response<http::string_body> HandleMapRequest(const http::request<Body, http::basic_fields<Allocator>>& req_, const RequestData& r_data_) {

        // ����������� �������
        std::cout << "Request method: " << req_.method_string() << std::endl;
        std::cout << "Request target: " << req_.target() << std::endl;

        json::value message;
        if (req_.method() != http::verb::get) {

            // ����������� ������
            std::cout << "Method not allowed: " << req_.method_string() << std::endl;

            return MakeResponse(http::status::method_not_allowed,
                Errors::GET_INVALID, req_.version(), req_.keep_alive(),
                ContentType::JSON,
                "no-cache"sv, "GET, HEAD"sv);
        }

        if (r_data_.r_target.empty()) {

            return MakeResponse(http::status::bad_request,
                Errors::BAD_REQ,
                req_.version(), req_.keep_alive(),
                ContentType::JSON,
                "no-cache"sv);
        }

        if (r_data_.r_target == "maps") {
            json::array array;
            for (auto& map : gs_.GetMaps()) {
                json::value val = { {"id",*map.GetId()}, {"name", map.GetName()} };
                array.push_back(val);
            }
            message = array;

            // ����������� ������
            std::cout << "Response status: " << statusClassToString(http::to_status_class(http::status::ok)) << std::endl;

            return MakeResponse(http::status::ok,
                json::serialize(message),
                req_.version(), req_.keep_alive(),
                ContentType::JSON, "no-cache"sv);
        }
        else {
            json::object resp_message;
            model::Map::Id id_{ r_data_.r_target };
            std::shared_ptr<model::Map> map = gs_.FindMap(id_);
            if (map != nullptr) {
                std::vector<std::string> keys_in_map = map->GetKeys();
                for (const auto& key : keys_in_map) {
                    if (key == "id") {
                        resp_message["id"] = *map->GetId();
                    }
                    else if (key == "name") {
                        resp_message["name"] = map->GetName();
                    }
                    else if (key == "roads") {
                        boost::json::array roads = PrepareRoadsForResponse(map).as_array();
                        resp_message["roads"] = roads;
                    }
                    else if (key == "buildings") {
                        boost::json::array buildings = PrepareBuildingsForResponse(map).as_array();
                        resp_message["buildings"] = buildings;
                    }
                    else if (key == "offices") {
                        boost::json::array offices = PrepareOfficesForResponse(map).as_array();
                        resp_message["offices"] = offices;
                    }
                    else if (key == "lootTypes") {
                        boost::json::array lootTypes = PrepareLootTypesForResponse(map).as_array();
                        resp_message["lootTypes"] = lootTypes;
                    }
                }
                message = resp_message;

                // ����������� ������
                std::cout << "Response status: " << statusClassToString(http::to_status_class(http::status::ok)) << std::endl;

                return MakeResponse(http::status::ok,
                    json::serialize(message),
                    req_.version(), req_.keep_alive(),
                    ContentType::JSON,
                    "no-cache"sv);

            }
            else {
                return MakeResponse(http::status::not_found,
                    Errors::MAP_NOT_FOUND,
                    req_.version(), req_.keep_alive(),
                    ContentType::JSON,
                    "no-cache"sv);

            }
        }
    }

    template <typename Body, typename Allocator>
    http::response<http::string_body> HandlePlayerJoinRequest(const http::request<Body, http::basic_fields<Allocator>>& req_) {

        // ����������� �������
        std::cout << "Request method: " << req_.method_string() << std::endl;
        std::cout << "Request target: " << req_.target() << std::endl;

        if (req_.method() != http::verb::post) {

            // ����������� ������
            std::cout << "Method not allowed: " << req_.method_string() << std::endl;

            return MakeResponse(http::status::method_not_allowed, 
                                Errors::POST_INVALID, 
                                req_.version(), req_.keep_alive(), 
                                ContentType::JSON, 
                                "no-cache"sv, "POST"sv);
        }
        std::string user_name;
        std::string map_id;
        try {
            boost::json::value parsed_req = boost::json::parse(req_.body());
            if (parsed_req.as_object().find("userName") == parsed_req.as_object().end() || parsed_req.as_object().at("userName").as_string().empty()) {
                return MakeResponse(http::status::bad_request, 
                                    Errors::USERNAME_EMPTY, 
                                    req_.version(), req_.keep_alive(), 
                                    ContentType::JSON, 
                                    "no-cache"sv);
            }
            user_name = parsed_req.as_object().at("userName").as_string();
            map_id = parsed_req.as_object().at("mapId").as_string();
        } catch (const std::exception& ex) {
            return MakeResponse(http::status::bad_request, 
                                Errors::PARSING_ERROR, 
                                req_.version(), req_.keep_alive(), 
                                ContentType::JSON, 
                                "no-cache"sv);
        }
        model::Map::Id mapId(map_id);
        auto map = gs_.FindMap(mapId);
        if (map == nullptr) {
            return MakeResponse(http::status::not_found, Errors::MAP_NOT_FOUND, req_.version(), req_.keep_alive(), ContentType::JSON, "no-cache"sv);
        }
        boost::json::object resp;
        try {
            std::shared_ptr<const model::Player> player = gs_.JoinGame(model::Map::Id(mapId), user_name);
            resp = {{"authToken", *player->GetPlayerToken()},
                    {"playerId", player->GetId()}};
        } catch (const std::exception& ex) {
            return MakeResponse(http::status::internal_server_error, 
                                "Join game failed: "s + ex.what(), 
                                req_.version(), req_.keep_alive(), 
                                ContentType::HTML);
        }

        // ����������� ������
        std::cout << "Response status: " << statusClassToString(http::to_status_class(http::status::ok)) << std::endl;

        return MakeResponse(http::status::ok, 
                            boost::json::serialize(resp), 
                            req_.version(), req_.keep_alive(), 
                            ContentType::JSON, 
                            "no-cache"sv);
    }

    template <typename Body, typename Allocator>
    http::response<http::string_body> HandleTickRequest(const http::request<Body, http::basic_fields<Allocator>>& req_) {
        if (req_.method() != http::verb::post) {
            return MakeResponse(http::status::method_not_allowed, 
                                Errors::INVALID_METHOD, 
                                req_.version(), req_.keep_alive(), 
                                ContentType::JSON, 
                                "no-cache"sv, "POST"sv);            
        }
        double delta_t;
        try {
            json::value parsed_req = json::parse(req_.body());
            if (parsed_req.as_object().find("timeDelta") == parsed_req.as_object().end()) {
                return MakeResponse(http::status::bad_request, 
                                    Errors::BAD_REQ, 
                                    req_.version(), req_.keep_alive(), 
                                    ContentType::JSON, 
                                    "no-cache"sv);
            }
            delta_t = parsed_req.as_object().at("timeDelta").as_int64() / 1000.;
        } catch (const std::exception& ex) {
            return MakeResponse(http::status::bad_request, 
                                Errors::PARSING_ERROR, 
                                req_.version(), req_.keep_alive(), 
                                ContentType::JSON, 
                                "no-cache"sv);
        }
        json::object resp;
        try {
            gs_.SetGameServerTick(delta_t);
        } catch (const std::exception& ex) {
            throw;
        }
        return MakeResponse(http::status::ok, 
                            boost::json::serialize(resp), 
                            req_.version(), req_.keep_alive(), 
                            ContentType::JSON, 
                            "no-cache"sv);
    }

// Methods, authorization required ->

    template <typename Body, typename Allocator>
    http::response<http::string_body> HandlePlayersListRequest(const http::request<Body, http::basic_fields<Allocator>>& req_) {
        if (req_.method() != http::verb::get && req_.method() != http::verb::head) {
            return MakeResponse(http::status::method_not_allowed, 
                                Errors::INVALID_METHOD, 
                                req_.version(), req_.keep_alive(), 
                                ContentType::JSON, 
                                "no-cache"sv, "GET, HEAD"sv);            
        } 
        boost::json::object resp;
        return ExecuteAuthorized([this, &resp, &req_](std::shared_ptr<const model::Player> player) {
            for (auto pl : gs_.GetPlayers()) {
                if (pl.second->GetPlayersSession() == player->GetPlayersSession()) {
                    std::shared_ptr<model::Player> plr = pl.second;
                    resp[std::to_string(plr->GetId())] = boost::json::object{{"name", plr->GetName()}};
                }
            }
            return MakeResponse(http::status::ok, 
                                boost::json::serialize(resp), 
                                req_.version(), req_.keep_alive(), 
                                ContentType::JSON, 
                                "no-cache"sv);
        }, req_);
    }

    template <typename Body, typename Allocator>
    http::response<http::string_body> HandleStateRequest(const http::request<Body, http::basic_fields<Allocator>>& req_) {
        if (req_.method() != http::verb::get && req_.method() != http::verb::head) {
            return MakeResponse(http::status::method_not_allowed, 
                                Errors::INVALID_METHOD, 
                                req_.version(), req_.keep_alive(), 
                                ContentType::JSON, 
                                "no-cache"sv, "GET, HEAD"sv);
        }
        boost::json::object resp;
        return ExecuteAuthorized([this, &resp, &req_](std::shared_ptr<const model::Player> player) {
            
            for (auto pl : gs_.GetPlayers()) {
                if (pl.second->GetPlayersSession() == player->GetPlayersSession()) {
                    resp[std::to_string(pl.second->GetId())] = {
                        {"dir", pl.second->GetDog()->GetDogDirection()},
                        {"pos", {static_cast<double>(pl.second->GetDog()->GetDogPosition().x_), 
                                 static_cast<double>(pl.second->GetDog()->GetDogPosition().y_)}},
                        {"speed", {static_cast<double>(pl.second->GetDog()->GetDogSpeed().x_),
                                   static_cast<double>(pl.second->GetDog()->GetDogSpeed().y_)}}
                    };
                }
            }
            boost::json::object resp_message {{"players", resp}};
            return MakeResponse(http::status::ok, 
                                json::serialize(resp_message), 
                                req_.version(), req_.keep_alive(), 
                                ContentType::JSON, 
                                "no-cache"sv);
        }, req_); 
    }

    template <typename Body, typename Allocator>
    http::response<http::string_body> HandleActionRequest(const http::request<Body, http::basic_fields<Allocator>>& req_) {
        if (req_.method() != http::verb::post) {
            return MakeResponse(http::status::method_not_allowed, 
                                Errors::INVALID_METHOD, 
                                req_.version(), req_.keep_alive(), 
                                ContentType::JSON, 
                                "no-cache"sv, "POST"sv);
        }
        return ExecuteAuthorized([this, &req_](std::shared_ptr<const model::Player> player) {
            try {
                json::value parsed_req = json::parse(req_.body());
                player->GetDog()->SetDogDirection(static_cast<std::string>(parsed_req.as_object().at("move").as_string()));
            } catch (const std::exception& ex) {
                return MakeResponse(http::status::bad_request, 
                                    Errors::ACTION_PARSING_ERROR, 
                                    req_.version(), req_.keep_alive(), 
                                    ContentType::JSON, 
                                    "no-cache"sv);
            }
            json::object resp;
            return MakeResponse(http::status::ok, 
                                json::serialize(resp), 
                                req_.version(), req_.keep_alive(), 
                                ContentType::JSON, 
                                "no-cache"sv);
        }, req_); 
    }

private:
    GameServer& gs_;

    template <typename Body, typename Allocator>
    std::optional<model::Token> TryExtractToken(const http::request<Body, http::basic_fields<Allocator>>& req_) {
        std::string authorization;
        if (req_.count(http::field::authorization)) {
            authorization = req_.at(http::field::authorization);
        } else {
            return std::nullopt;
        }
        const std::string auth_prefix = "Bearer ";
        if (authorization.starts_with(auth_prefix)) {
            authorization = authorization.substr(auth_prefix.size());
        } else {
            return std::nullopt;
        }
        std::transform(authorization.begin(), authorization.end(), authorization.begin(), 
            [](unsigned char c) {
                return std::tolower(c);
            });
        if (authorization.size() != 32) {
            return std::nullopt;
        }
        // for (auto c : authorization) {
        //     if (!isxdigit(c)) {
        //         return std::nullopt;
        //     }
        // }
        if (std::any_of(authorization.begin(), authorization.end(), [](auto c){return !isxdigit(c);})) {
            return std::nullopt;
        }
        return model::Token(authorization);
    }

    template <typename Fn, typename Body, typename Allocator>
    http::response<http::string_body> ExecuteAuthorized(Fn&& action, const http::request<Body, http::basic_fields<Allocator>>& req_) {
        bool keep_alive = req_.keep_alive();
        unsigned version = req_.version(); 
        if (auto token = this->TryExtractToken(req_)) {
            std::shared_ptr<const model::Player> pl = gs_.FindPlayer(*token);
            if (pl == nullptr) {
                return MakeResponse(http::status::unauthorized, 
                                    Errors::UNKNOWN_TOKEN, 
                                    version, keep_alive, 
                                    ContentType::JSON, 
                                    "no-cache"sv);
            }
            return action(pl);
        } else {
            return MakeResponse(http::status::unauthorized, 
                                Errors::INVALID_TOKEN, 
                                version, keep_alive, 
                                ContentType::JSON, 
                                "no-cache"sv);
        }
    }

};

}