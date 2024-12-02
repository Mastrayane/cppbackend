#pragma once
#ifndef __MODEL__LOOT_H__
#define __MODEL__LOOT_H__

#include <optional>
#include <string>
#include <compare>

#include "geometry.h"
#include "loot_type.h"
#include "game_object.h"


namespace model {

class Loot : public GameObject{
 public:
  Loot(const LootType& type,const Point2d& pos, const unsigned& id) : m_loot_type(type), GameObject(pos), m_id(id) {}

  const LootType& GetLootType() const noexcept { return m_loot_type;}
  const unsigned& GetId() const noexcept {return m_id;}

  bool operator==(const Loot& l) const {
    return m_id == l.m_id && m_loot_type.typeIndex == l.m_loot_type.typeIndex;
  }

  bool operator!=(const Loot& l) const {
    return !(*this == l);
  }

 private:
  const LootType m_loot_type;
  const unsigned m_id;

};

}  // namespace model

#endif  // __MODEL__LOOT_H__