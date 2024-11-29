#include "json_loader.h"

using namespace std::literals;

namespace json_loader {

    std::filesystem::path operator""_p(const char* data, std::size_t sz) {
        return std::filesystem::path(data, data + sz);
    }

    std::string LoadJsonFileAsString(const std::filesystem::path& json_path) {
        std::ifstream jsonfile;
        std::filesystem::path filepath = json_path;
        jsonfile.open(filepath);

        if (!jsonfile.is_open()) {
            throw std::runtime_error("Failed to open file: " + filepath.string());
        }

        std::stringstream buffer;
        buffer << jsonfile.rdbuf();
        jsonfile.close();
        return buffer.str();
    }

    void SetKeySequenceRoad(const boost::json::value& road, model::Road& road_) {
        for (const auto& pair : road.as_object()) {
            road_.SetKeySequence(pair.key_c_str());
        }
    }

    void AddRoadsToMap(const boost::json::value& parsed, model::Map& map) {
        for (auto& road : parsed.as_array()) {

            //std::cout << "Processing road: " << boost::json::serialize(road) << std::endl;

            if (road.as_object().contains(spati_keys::x_end)) {
                model::Road road_{ model::Road::HORIZONTAL,
                        {static_cast<int>(road.as_object().at(spati_keys::x_start).as_int64()),
                                            static_cast<int>(road.as_object().at(spati_keys::y_start).as_int64())},
                                            static_cast<int>(road.as_object().at(spati_keys::x_end).as_int64()) };
                SetKeySequenceRoad(road, road_);
                map.AddRoad(road_);
            }
            if (road.as_object().contains("y1")) {
                model::Road road_{ model::Road::VERTICAL,
                        {static_cast<int>(road.as_object().at(spati_keys::x_start).as_int64()),
                                            static_cast<int>(road.as_object().at(spati_keys::y_start).as_int64())},
                                            static_cast<int>(road.as_object().at(spati_keys::y_end).as_int64()) };
                SetKeySequenceRoad(road, road_);
                map.AddRoad(road_);
            }
        }
    }

    void AddBuildingsToMap(const boost::json::value& parsed, model::Map& map) {
        for (auto& building : parsed.as_array()) {

            //std::cout << "Processing building: " << boost::json::serialize(building) << std::endl;

            model::Rectangle rect{ {static_cast<int>(building.as_object().at(spati_keys::x_str).as_int64()),
                                   static_cast<int>(building.as_object().at(spati_keys::y_str).as_int64())},
                                  {static_cast<int>(building.as_object().at(spati_keys::w_str).as_int64()),
                                   static_cast<int>(building.as_object().at(spati_keys::h_str).as_int64())} };
            model::Building building_{ rect };
            for (const auto& pair : building.as_object()) {
                building_.SetKeySequence(pair.key_c_str());
            }
            map.AddBuilding(building_);
        }
    }

    void AddOfficesToMap(const boost::json::value& parsed, model::Map& map) {
        for (auto& office : parsed.as_array()) {

            //std::cout << "Processing office: " << boost::json::serialize(office) << std::endl;

            model::Office::Id id{ office.as_object().at("id").as_string().c_str() };
            model::Office office_{ id,
                                  {static_cast<int>(office.as_object().at(spati_keys::x_str).as_int64()),
                                   static_cast<int>(office.as_object().at(spati_keys::y_str).as_int64())},
                                  {static_cast<int>(office.as_object().at(spati_keys::x_offset).as_int64()),
                                   static_cast<int>(office.as_object().at(spati_keys::y_offset).as_int64())} };
            for (const auto& pair : office.as_object()) {
                office_.SetKeySequence(pair.key_c_str());
            } try {
                map.AddOffice(office_);
            }
            catch (std::invalid_argument& ex) {
                std::cerr << ex.what() << std::endl;
            }
        }
    }

    void SetDogSpeedToGame(const boost::json::value& parsed, model::Game& game) {
        if (parsed.as_object().contains("defaultDogSpeed")) {
            if (parsed.as_object().at("defaultDogSpeed").is_double()) {
                game.SetDefaultDogSpeed(parsed.as_object().at("defaultDogSpeed").as_double());
            }
        }
    }

    model::Game LoadGame(const std::filesystem::path& json_path) {
        // Загрузить содержимое файла json_path, например, в виде строки
        std::string json_as_string = LoadJsonFileAsString(json_path);

        // Добавляем логирование для проверки содержимого файла
        //std::cout << "Loaded JSON content: " << json_as_string << std::endl;

        boost::json::value parsed_json;
        try {
            parsed_json = boost::json::parse(json_as_string);
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing JSON: " << e.what() << std::endl;
            throw;
        }

        model::Game game;

        try {
            SetDogSpeedToGame(parsed_json.as_object(), game);
            AddMapsToGame(parsed_json.as_object().at("maps").as_array(), game);
        }
        catch (const std::exception& e) {
            std::cerr << "Error processing JSON: " << e.what() << std::endl;
            throw;
        }

        return game;
    }

    void SetLootGeneratorConfig(const boost::json::value& parsed, model::Game& game) {
        if (parsed.as_object().contains("lootGeneratorConfig")) {
            auto lootConfig = parsed.as_object().at("lootGeneratorConfig").as_object();
            double period = lootConfig.at("period").as_double();
            double probability = lootConfig.at("probability").as_double();
            game.SetLootGeneratorConfig(std::chrono::duration<double>(period), probability);
        }
    }

    void AddMapsToGame(const boost::json::value& parsed, model::Game& game) {
        for (auto& map : parsed.as_array()) {
            model::Map::Id id{ map.as_object().at("id").as_string().c_str() };
            model::Map map_i = model::Map{ id, map.as_object().at("name").as_string().c_str() };
            map_i.SetMapDogSpeed(game.GetDefaultDogSpeed());

            // Добавляем lootTypes
            std::vector<model::LootType> lootTypes;
            for (const auto& lootType : map.as_object().at("lootTypes").as_array()) {
                model::LootType lootTypeObj;
                lootTypeObj.name = lootType.as_object().at("name").as_string().c_str();
                lootTypeObj.file = lootType.as_object().at("file").as_string().c_str();
                lootTypeObj.type = lootType.as_object().at("type").as_string().c_str();
                lootTypeObj.rotation = lootType.as_object().at("rotation").as_int64();
                lootTypeObj.color = lootType.as_object().at("color").as_string().c_str();
                lootTypeObj.scale = lootType.as_object().at("scale").as_double();
                lootTypes.push_back(lootTypeObj);
            }
            map_i.AddLootTypes(lootTypes);

            for (const auto& pair : map.as_object()) {
                map_i.SetKeySequence(pair.key_c_str());
                if (pair.key() == "roads") {
                    AddRoadsToMap(map.as_object().at("roads").as_array(), map_i);
                }
                if (pair.key() == "buildings") {
                    AddBuildingsToMap(map.as_object().at("buildings").as_array(), map_i);
                }
                if (pair.key() == "offices") {
                    AddOfficesToMap(map.as_object().at("offices").as_array(), map_i);
                }
                if (pair.key() == "dogSpeed") {
                    map_i.SetMapDogSpeed(pair.value().as_double());
                }
            }
            game.AddMap(map_i);
        }
    }

    void AddLootTypesToMap(const boost::json::value& parsed, model::Map& map) {
        std::vector<model::LootType> lootTypes;
        for (const auto& lootType : parsed.as_object().at("lootTypes").as_array()) {
            model::LootType lootTypeObj;
            lootTypeObj.name = lootType.as_object().at("name").as_string().c_str();
            lootTypeObj.file = lootType.as_object().at("file").as_string().c_str();
            lootTypeObj.type = lootType.as_object().at("type").as_string().c_str();
            lootTypeObj.rotation = lootType.as_object().at("rotation").as_int64();
            lootTypeObj.color = lootType.as_object().at("color").as_string().c_str();
            lootTypeObj.scale = lootType.as_object().at("scale").as_double();
            lootTypes.push_back(lootTypeObj);
        }
        map.AddLootTypes(lootTypes);

        // Добавляем информацию о типах трофеев в extra_data
        extra_data::AddLootTypesForMap(*map.GetId(), parsed.as_object().at("lootTypes").as_array());
    }

}  // namespace json_loader