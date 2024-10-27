#include "json_loader.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace json_loader {

    namespace json = boost::json;

    model::Game LoadGame(const std::filesystem::path& json_path) {
        std::ifstream file(json_path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + json_path.string());
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string json_content = buffer.str();

        json::value json_value = json::parse(json_content);
        model::Game game;

        if (json_value.is_object() && json_value.as_object().contains("maps")) {
            const json::array& maps_array = json_value.as_object().at("maps").as_array();

            for (const auto& map_value : maps_array) {
                if (map_value.is_object()) {
                    const json::object& map_obj = map_value.as_object();

                    model::Map::Id map_id{ map_obj.at("id").as_string().c_str() };
                    std::string map_name = map_obj.at("name").as_string().c_str();
                    model::Map map(map_id, map_name);

                    std::cout << "Loading map: " << *map.GetId() << " - " << map.GetName() << std::endl;

                    // Загрузка дорог
                    if (map_obj.contains("roads")) {
                        const json::array& roads_array = map_obj.at("roads").as_array();
                        for (const auto& road_value : roads_array) {
                            const json::object& road_obj = road_value.as_object();
                            model::Point start{ static_cast<model::Coord>(road_obj.at("x0").as_int64()), static_cast<model::Coord>(road_obj.at("y0").as_int64()) };
                            if (road_obj.contains("x1")) {
                                model::Coord end_x = static_cast<model::Coord>(road_obj.at("x1").as_int64());
                                map.AddRoad(model::Road(model::Road::HORIZONTAL, start, end_x));
                            }
                            else if (road_obj.contains("y1")) {
                                model::Coord end_y = static_cast<model::Coord>(road_obj.at("y1").as_int64());
                                map.AddRoad(model::Road(model::Road::VERTICAL, start, end_y));
                            }
                        }
                    }

                    // Загрузка зданий
                    if (map_obj.contains("buildings")) {
                        const json::array& buildings_array = map_obj.at("buildings").as_array();
                        for (const auto& building_value : buildings_array) {
                            const json::object& building_obj = building_value.as_object();
                            model::Point position{ static_cast<model::Coord>(building_obj.at("x").as_int64()), static_cast<model::Coord>(building_obj.at("y").as_int64()) };
                            model::Size size{ static_cast<model::Dimension>(building_obj.at("w").as_int64()), static_cast<model::Dimension>(building_obj.at("h").as_int64()) };
                            model::Rectangle bounds{ position, size };
                            map.AddBuilding(model::Building(bounds));
                        }
                    }

                    // Загрузка офисов
                    if (map_obj.contains("offices")) {
                        const json::array& offices_array = map_obj.at("offices").as_array();
                        for (const auto& office_value : offices_array) {
                            const json::object& office_obj = office_value.as_object();
                            model::Office::Id office_id{ office_obj.at("id").as_string().c_str() };
                            model::Point position{ static_cast<model::Coord>(office_obj.at("x").as_int64()), static_cast<model::Coord>(office_obj.at("y").as_int64()) };
                            model::Offset offset{ static_cast<model::Dimension>(office_obj.at("offsetX").as_int64()), static_cast<model::Dimension>(office_obj.at("offsetY").as_int64()) };
                            map.AddOffice(model::Office(office_id, position, offset));
                        }
                    }

                    game.AddMap(std::move(map));
                }
            }
        }

        return game;
    }

}  // namespace json_loader