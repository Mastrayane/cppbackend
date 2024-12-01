#include "collector.h"

namespace model {

CollisionResult TryCollectPoint(Point2d a, Point2d b, Point2d c) {
  // Проверим, что перемещение ненулевое.
  // Тут приходится использовать строгое равенство, а не приближённое,
  // пскольку при сборе заказов придётся учитывать перемещение даже на небольшое
  // расстояние.
  assert(b.x != a.x || b.y != a.y);
  const double u_x = c.x - a.x;
  const double u_y = c.y - a.y;
  const double v_x = b.x - a.x;
  const double v_y = b.y - a.y;
  const double u_dot_v = u_x * v_x + u_y * v_y;
  const double u_len2 = u_x * u_x + u_y * u_y;
  const double v_len2 = v_x * v_x + v_y * v_y;
  const double proj_ratio = u_dot_v / v_len2;
  const double sq_distance = u_len2 - (u_dot_v * u_dot_v) / v_len2;

  return CollisionResult(sq_distance, proj_ratio);
}

std::list<CollisionEvent> Collector::FindEvent() const {
  std::list<CollisionEvent> event;

  for (auto& dogmove : m_dogs) {
    // handle loot collision
    for (auto& li : m_loots) {
      auto colres = TryCollectPoint(dogmove.dog->GetPosition(), dogmove.nextPos, li.GetPosition());
      if (colres.IsIntersect(dogmove.dog->GetWidth() + li.GetWidth())) {
        event.emplace_back(dogmove.dog, li, colres.sq_distance, colres.proj_ratio);
      }
    }

    // handle office collision
    for (auto& oi : m_offices) {
      auto colres = TryCollectPoint(dogmove.dog->GetPosition(), dogmove.nextPos, oi.GetPosition());
      if (colres.IsIntersect(dogmove.dog->GetWidth() + oi.GetWidth())) {
        event.emplace_back(dogmove.dog, oi, colres.sq_distance, colres.proj_ratio);
      }
    }
  }

  event.sort([](const CollisionEvent& a, const CollisionEvent& b) { return a.time < b.time; });

  return event;
}

void Collector::CollisionEventHandler() {
  std::list<CollisionEvent> event = FindEvent();

  for (auto it = event.begin(); it != event.end(); it = event.erase(it)) {
    // if Office need unload and calculte points
    if (std::holds_alternative<Office>(it->obj)) {
      // get loots
      auto loots = it->dog->UnloadLoots();

      // calculate
      auto points = std::accumulate(loots.begin(), loots.end(), 0, [](const int& a, const Loot& l) {
        auto val = l.GetLootType().value;
        if (val) {
          return static_cast<int>(a + *val);
        }
        return a;
      });

      // add points
      it->dog->AddPoints(points);

    } else if (std::holds_alternative<Loot>(it->obj) && !it->dog->IsFull()) {
      Loot l = std::get<Loot>(it->obj);
      it->dog->AddLoot(l);

      // remove collected loots from list
      m_loots.remove(l);

      // remove event with collected loot
      for (auto j = std::next(it); j != event.end();) {
        if (std::holds_alternative<Loot>(j->obj)) {
          if (std::get<Loot>(j->obj) == l) {
            j = event.erase(j);
          } else {
            ++j;
          }
        }
      }
    }
  }
}

}  // namespace model
