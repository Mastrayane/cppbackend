#pragma once

#include <boost/json.hpp>
#include <unordered_map>
#include <string>

namespace extra_data {

	using MapId = std::string;

	// Хранилище для информации о типах трофеев
	extern std::unordered_map<MapId, boost::json::array> lootTypesData;

	// Функция для добавления информации о типах трофеев для конкретной карты
	void AddLootTypesForMap(const MapId& mapId, const boost::json::array& lootTypes);

	// Функция для получения информации о типах трофеев для конкретной карты
	const boost::json::array& GetLootTypesForMap(const MapId& mapId);

} // namespace extra_data