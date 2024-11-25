#include "api_handler.h"
#include "json_loader.h"

namespace http_handler {

    using namespace spati_keys;

    boost::json::value PrepareRoadsForResponse(std::shared_ptr<model::Map> map) {
        boost::json::array roads;
        for (const auto& road : map->GetRoads()) {
            boost::json::object road_;
            for (const std::string& str : road.GetKeys()) {
                if (str == x_start) { road_[x_start] = road.GetStart().x; }
                if (str == x_end) { road_[x_end] = road.GetEnd().x; }
                if (str == y_start) { road_[y_start] = road.GetStart().y; }
                if (str == y_end) { road_[y_end] = road.GetEnd().y; }
            }
            roads.push_back(road_);
        }
        return roads;
    }

    boost::json::value PrepareBuildingsForResponse(std::shared_ptr<model::Map> map) {
        boost::json::array buildings;
        for (const auto& building : map->GetBuildings()) {
            boost::json::object build_;
            for (const std::string& str : building.GetKeys()) {
                if (str == x_str) { build_[x_str] = building.GetBounds().position.x; }
                if (str == y_str) { build_[y_str] = building.GetBounds().position.y; }
                if (str == w_str) { build_[w_str] = building.GetBounds().size.width; }
                if (str == h_str) { build_[h_str] = building.GetBounds().size.height; }
            }
            buildings.push_back(build_);
        }
        return buildings;
    }

    boost::json::value PrepareOfficesForResponse(std::shared_ptr<model::Map> map) {
        boost::json::array offices;
        for (const auto& office : map->GetOffices()) {
            boost::json::object office_;
            for (const std::string& str : office.GetKeys()) {
                if (str == "id") { office_["id"] = *office.GetId(); }
                if (str == x_str) { office_[x_str] = office.GetPosition().x; }
                if (str == y_str) { office_[y_str] = office.GetPosition().y; }
                if (str == x_offset) { office_[x_offset] = office.GetOffset().dx; }
                if (str == y_offset) { office_[y_offset] = office.GetOffset().dy; }
            }
            offices.push_back(office_);
        }
        return offices;
    }

    boost::json::value PrepareMapForResponse(std::shared_ptr<model::Map> map) {
        boost::json::object mapObj;
        mapObj["id"] = *map->GetId();
        mapObj["name"] = map->GetName();
        mapObj["roads"] = PrepareRoadsForResponse(map);
        mapObj["buildings"] = PrepareBuildingsForResponse(map);
        mapObj["offices"] = PrepareOfficesForResponse(map);

        // Добавляем информацию о lootTypes
        boost::json::array lootTypesArray;
        for (const auto& lootType : map->GetLootTypes()) {
            lootTypesArray.push_back(boost::json::value_from(lootType));
        }
        mapObj["lootTypes"] = lootTypesArray;

        return mapObj;
    }

    boost::json::value PrepareGameStateForResponse(std::shared_ptr<model::GameSession> session) {
        boost::json::object stateObj;
        // ... (другие поля состояния игры)

        // Добавляем информацию о потерянных предметах
        boost::json::object lostObjectsObj;
        for (const auto& [id, loot] : session->GetMap()->GetLostObjects()) {
            boost::json::object lootObj;
            lootObj["type"] = loot.first;
            lootObj["pos"] = { loot.second.x, loot.second.y };
            lostObjectsObj[std::to_string(id)] = lootObj;
        }
        stateObj["lostObjects"] = lostObjectsObj;

        return stateObj;
    }

} // namespace http_handler