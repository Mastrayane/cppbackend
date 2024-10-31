#include "request_handler.h"
#include <iostream>
#include <algorithm>
#include "visitor.h"

namespace http_handler {

    json::object RequestHandler::SerializeMap(const model::Map& map) {
        json::object mapJson = {
            {"id", *map.GetId()},
            {"name", map.GetName()},
            {"roads", json::array()},
            {"buildings", json::array()},
            {"offices", json::array()}
        };

        JsonVisitor visitor(mapJson);

        for (const auto& road : map.GetRoads()) {
            road.Accept(visitor);
        }

        for (const auto& building : map.GetBuildings()) {
            building.Accept(visitor);
        }

        for (const auto& office : map.GetOffices()) {
            office.Accept(visitor);
        }

        return mapJson;
    }

}  // namespace http_handler