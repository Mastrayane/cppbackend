#include "game_session.h"

#include <random>

#include "../logger.h"
#include "collector.h"

namespace model {

namespace {

  // get random number between [0, max]
  unsigned int Rand( unsigned max) {
    if (max == 0) return max;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0,max);
    return distribution(generator);
  }

  /// decreasing the minimum point and increasing the maximum point 
  /// depending on the road boundaries
  inline void BoundExpand(const Road& road, Point2d& min, Point2d& max) {
    // change min
    min.x = std::min(road.GetMinX(), min.x);
    min.y = std::min(road.GetMinY(), min.y);
    // change max
    max.x = std::max(road.GetMaxX(), max.x);
    max.y = std::max(road.GetMaxY(), max.y);
  }

}

GameSession::GameSession(const Map& map, LootGenerator gen) : m_map(map), m_random_spawn(false), m_loot_gen(std::move(gen)) {
}

GameSession::~GameSession() {
}

const Map& GameSession::GetMap() const noexcept {
  return m_map;
}

void GameSession::AddDog(std::shared_ptr<Dog> dog) {
  dog->SetPosition(m_map.GetRandPoint(m_random_spawn));
  dog->SetDefaultSpeed(m_map.GetDogSpeed());
  dog->SetBagSize(m_map.GetBagCapacity());
  m_dogs.emplace_back(dog);
}

void GameSession::Update(const double& delta) {
  GenerateLoot(delta);

  DogsUpdate(delta);
}

void GameSession::SetDogRandomSpawn(bool enable){
  m_random_spawn = enable;
}

void GameSession::GenerateLoot(const double& delta)
{
  const size_t max_loots = m_map.GetLootTypes().size(); 
  if(max_loots == 0 || m_loots.size() >= m_dogs.size()) return;

  const std::chrono::milliseconds ms (static_cast<int64_t>(delta*1000));

  const unsigned count = m_loot_gen.Generate(ms, m_loots.size(), m_dogs.size());

  for(size_t i = 0; i < count; ++i) {
    const unsigned loot_index = Rand(max_loots-1);
    assert(loot_index <= m_map.GetLootTypes().size());
    m_loots.emplace_back(m_map.GetLootTypes().at(loot_index), m_map.GetRandPoint(), m_loot_id++);
  }
}

void GameSession::DogsUpdate(const double& delta)
{
  Collector collector(m_loots, m_map.GetOffices());
  // dog udate
  for (auto it = m_dogs.begin(); it != m_dogs.end(); ++it) {
    auto dog = *it;
    assert(dog != nullptr);

    const Point2d& pos = dog->GetPosition();
    const Point2d& speed = dog->GetSpeed();

    if(speed == Point2d()) {
      continue;
    }

    Point2d nextPos = pos + (speed * delta);



    // border define
    Point2d min_pos = pos; 
    Point2d max_pos = pos;

    // border update
    for( auto road = m_map.GetRoads().cbegin(); road != m_map.GetRoads().cend(); ++road) {
      if(road->Contains(pos)) {
        BoundExpand(*road, min_pos, max_pos);
      }
    }

    // next position slice if collision detect
    if (nextPos.y < min_pos.y) {
      nextPos.y = min_pos.y;
      dog->Stop();
    } else if (nextPos.y > max_pos.y) {
      nextPos.y = max_pos.y;
      dog->Stop();
    } else if (nextPos.x < min_pos.x) {
      nextPos.x = min_pos.x;
      dog->Stop();
    } else if (nextPos.x > max_pos.x) {
      nextPos.x = max_pos.x;
      dog->Stop();
    }

    if(pos == nextPos) {
      continue;
    }

    collector.AddDogToMoveUpdate(dog, nextPos);

    // dog->SetPosition(nextPos);
  }
  collector.Update();
}


}  // namespace model
