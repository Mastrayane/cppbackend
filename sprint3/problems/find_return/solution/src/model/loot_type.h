#ifndef __LOOT_TYPE_H__
#define __LOOT_TYPE_H__

#include <compare>
#include <optional>
#include <string>

namespace model
{
  struct LootType {
  std::optional<std::string> name = std::nullopt;   // example -> = "key";
  std::optional<std::string> file = std::nullopt;   // example -> = "assets/key.obj";
  std::optional<std::string> type = std::nullopt;   // example -> = "obj";
  std::optional<int> rotation = std::nullopt;       // example -> = 90;
  std::optional<std::string> color = std::nullopt;  // example -> = "#338844";
  std::optional<double> scale = std::nullopt;       // example -> = 0.03;
  std::optional<size_t> value = std::nullopt;

  int typeIndex = -1;

  auto operator<=>(const LootType&) const = default;
};

} // namespace model


#endif // __LOOT_TYPE_H__