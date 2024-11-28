#include "extra_data.h"

namespace extra_data {

    std::unordered_map<MapId, boost::json::array> lootTypesData;

    void AddLootTypesForMap(const MapId& mapId, const boost::json::array& lootTypes) {
        lootTypesData[mapId] = lootTypes;
    }

    const boost::json::array& GetLootTypesForMap(const MapId& mapId) {
        return lootTypesData.at(mapId);
    }

} // namespace extra_data