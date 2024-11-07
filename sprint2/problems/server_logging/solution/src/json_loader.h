#pragma once

#include <filesystem>
#include <boost/json.hpp>
#include "model.h"

namespace json_loader {

    model::Game LoadGame(const std::filesystem::path& json_path);

    namespace detail {
        std::string ReadFileContent(const std::filesystem::path& json_path);
        void LoadRoads(const boost::json::object& map_obj, model::Map& map);
        void LoadBuildings(const boost::json::object& map_obj, model::Map& map);
        void LoadOffices(const boost::json::object& map_obj, model::Map& map);
    } // namespace detail

}  // namespace json_loader