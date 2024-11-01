#include "request_handler.h"
#include <iostream>
#include <algorithm>
#include "serializer.h"

namespace http_handler {

    json::object RequestHandler::SerializeMap(const model::Map& map) {
        json::object serializedMap = {
            {"id", *map.GetId()},
            {"name", map.GetName()},
            {"roads", json::array()},
            {"buildings", json::array()},
            {"offices", json::array()}
        };

        JsonSerializer serializer(serializedMap);

        for (const auto& road : map.GetRoads()) {
            road.Accept(serializer);
        }

        for (const auto& building : map.GetBuildings()) {
            building.Accept(serializer);
        }

        for (const auto& office : map.GetOffices()) {
            office.Accept(serializer);
        }

        return serializedMap;
    }

}  // namespace http_handler