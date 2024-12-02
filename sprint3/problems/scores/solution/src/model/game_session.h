#ifndef __MODEL_GSES_H__
#define __MODEL_GSES_H__

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <list>

#include "../tagged.h"
#include "dog.h"
#include "model_map.h"
#include "loot_generator.h"
#include "loot.h"

namespace model {

class GameSession {
 public:
  // using LootContainer = std::list<std::unique_ptr<Loot>>;

  explicit GameSession(const Map& map, LootGenerator gen);
  ~GameSession();

  const Map& GetMap() const noexcept;

  void AddDog(std::shared_ptr<Dog> dog);

  void Update(double delta_time);

  void SetDogRandomSpawn(bool enable = true);

  const std::list<Loot>& GetLoots() const noexcept { return m_loots;}
  //const std::list<std::shared_ptr<GameObject>>& GetObjects() const noexcept { return m_objects;}

 private:

  void GenerateLoot(double delta_time);

  void DogsUpdate(double delta_time);

  const Map& m_map;
  std::vector<std::shared_ptr<Dog>> m_dogs;
  bool m_random_spawn;

  LootGenerator m_loot_gen;

  std::list<Loot> m_loots;

  int m_loot_id = 0;
};

}  // namespace model

#endif  // __MODEL_GSES_H__