#include "request_handler.h"
#include <iostream>
#include <algorithm>

namespace http_handler {

    json::object RequestHandler::SerializeMap(const model::Map& map) {
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

        return mapJson;
    }

}  // namespace http_handler