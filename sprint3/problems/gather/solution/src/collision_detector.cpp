#include "collision_detector.h"

#include <cassert>
#include <algorithm>

namespace collision_detector {

CollectionResult TryCollectPoint(geom::Point2D a, geom::Point2D b, geom::Point2D c) {
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

    return CollectionResult(sq_distance, proj_ratio);
}

// В задании на разработку тестов реализовывать следующую функцию не нужно -
// она будет линковаться извне.
/**/
std::vector<GatheringEvent> FindGatherEvents(const ItemGathererProvider& provider) {
  std::vector<GatheringEvent> ret;
  for (size_t i = 0; i < provider.ItemsCount(); i++)  {
    const Item& item = provider.GetItem(i);
    for (size_t j = 0; j < provider.GatherersCount(); j++) {
      const Gatherer& gather = provider.GetGatherer(j);
      if(gather.start_pos == gather.end_pos) {
        continue;
      }
      auto result = TryCollectPoint(gather.start_pos, gather.end_pos, item.position);
      if(result.IsCollected(gather.width + item.width)) {
        GatheringEvent event;
        event.gatherer_id = j;
        event.item_id = i;
        event.sq_distance = result.sq_distance;
        event.time = result.proj_ratio;
        ret.push_back(event);
      }
    }
  }
  std::sort(ret.begin(), ret.end(), [](const GatheringEvent& a, const GatheringEvent& b){
    return a.time < b.time;
  });
  return ret;
}
/**/

}  // namespace collision_detector