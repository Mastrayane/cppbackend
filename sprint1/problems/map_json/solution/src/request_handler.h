#pragma once
#include "http_server.h"
#include "model.h"
#include <boost/json.hpp>
#include <string_view>

namespace http_handler {
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace json = boost::json;

    class RequestHandler {
    public:
        explicit RequestHandler(model::Game& game)
            : game_{ game } {
        }

        RequestHandler(const RequestHandler&) = delete;
        RequestHandler& operator=(const RequestHandler&) = delete;

        template <typename Body, typename Allocator, typename Send>
        void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
            // Обработать запрос request и отправить ответ, используя send
            if (req.method() == http::verb::get) {
                if (req.target() == "/api/v1/maps") {
                    HandleGetMaps(std::forward<Send>(send));
                }
                else if (req.target().starts_with("/api/v1/maps/")) {
                    HandleGetMapById(req.target().substr(12), std::forward<Send>(send));
                }
                else {
                    SendBadRequest(std::forward<Send>(send));
                }
            }
            else {
                SendBadRequest(std::forward<Send>(send));
            }
        }

    private:
        template <typename Send>
        void HandleGetMaps(Send&& send) {
            json::array mapsJson;
            for (const auto& map : game_.GetMaps()) {
                mapsJson.emplace_back(json::object{
                    {"id", *map.GetId()},
                    {"name", map.GetName()}
                    });
            }

            http::response<http::string_body> res{ http::status::ok, 11 };
            res.set(http::field::content_type, "application/json");
            res.body() = json::serialize(mapsJson);
            res.prepare_payload();
            send(std::move(res));
        }

        template <typename Send>
        void HandleGetMapById(std::string_view mapId, Send&& send) {
            std::cout << "Handling request for map: " << mapId << std::endl;

            // Удаляем лишние символы, такие как '/'
            std::string cleanedMapId(mapId);
            cleanedMapId.erase(std::remove(cleanedMapId.begin(), cleanedMapId.end(), '/'), cleanedMapId.end());

            model::Map::Id id{ cleanedMapId };
            std::cout << "Searching for map with id: " << *id << std::endl;

            if (auto mapPtr = game_.FindMap(id); mapPtr) {
                const auto& map = *mapPtr;
                std::cout << "Found map: " << *map.GetId() << " - " << map.GetName() << std::endl;

                json::object mapJson = {
                    {"id", *map.GetId()},
                    {"name", map.GetName()},
                    {"roads", json::array()},
                    {"buildings", json::array()},
                    {"offices", json::array()}
                };

                for (const auto& road : map.GetRoads()) {
                    if (road.IsHorizontal()) {
                        mapJson.at("roads").as_array().emplace_back(json::object{
                            {"x0", road.GetStart().x},
                            {"y0", road.GetStart().y},
                            {"x1", road.GetEnd().x}
                            });
                    }
                    else if (road.IsVertical()) {
                        mapJson.at("roads").as_array().emplace_back(json::object{
                            {"x0", road.GetStart().x},
                            {"y0", road.GetStart().y},
                            {"y1", road.GetEnd().y}
                            });
                    }
                }

                for (const auto& building : map.GetBuildings()) {
                    const auto& bounds = building.GetBounds();
                    mapJson.at("buildings").as_array().emplace_back(json::object{
                        {"x", bounds.position.x},
                        {"y", bounds.position.y},
                        {"w", bounds.size.width},
                        {"h", bounds.size.height}
                        });
                }

                for (const auto& office : map.GetOffices()) {
                    mapJson.at("offices").as_array().emplace_back(json::object{
                        {"id", *office.GetId()},
                        {"x", office.GetPosition().x},
                        {"y", office.GetPosition().y},
                        {"offsetX", office.GetOffset().dx},
                        {"offsetY", office.GetOffset().dy}
                        });
                }

                http::response<http::string_body> res{ http::status::ok, 11 };
                res.set(http::field::content_type, "application/json");
                res.body() = json::serialize(mapJson);
                res.prepare_payload();
                send(std::move(res));
            }
            else {
                std::cout << "Map not found: " << cleanedMapId << std::endl;
                SendNotFound(std::forward<Send>(send));
            }
        }

        template <typename Send>
        void SendBadRequest(Send&& send) {
            json::object errorJson = {
                {"code", "badRequest"},
                {"message", "Bad request"}
            };

            http::response<http::string_body> res{ http::status::bad_request, 11 };
            res.set(http::field::content_type, "application/json");
            res.body() = json::serialize(errorJson);
            res.prepare_payload();
            send(std::move(res));
        }

        template <typename Send>
        void SendNotFound(Send&& send) {
            json::object errorJson = {
                {"code", "mapNotFound"},
                {"message", "Map not found"}
            };

            http::response<http::string_body> res{ http::status::not_found, 11 };
            res.set(http::field::content_type, "application/json");
            res.body() = json::serialize(errorJson);
            res.prepare_payload();
            send(std::move(res));
        }

        model::Game& game_;
    };

}  // namespace http_handler