#define _USE_MATH_DEFINES

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
// #include <format>

#include <random>

#include "../src/collision_detector.h"

using namespace std::literals;
// Напишите здесь тесты для функции collision_detector::FindGatherEvents

namespace Catch {
template <>
struct StringMaker<collision_detector::GatheringEvent> {
  static std::string convert(collision_detector::GatheringEvent const& value) {
    //     return std::format("({},{},{},{})", value.gatherer_id, value.item_id, value.sq_distance, value.time);
    return "("s + std::to_string(value.gatherer_id) +
           ", "s + std::to_string(value.item_id) +
           ", "s + std::to_string(value.sq_distance) +
           ", "s + std::to_string(value.time) +
           ")"s;
  }
};
}  // namespace Catch

using namespace collision_detector;
using namespace geom;

class ItemGatherer : public ItemGathererProvider {
  public:
  virtual size_t ItemsCount() const {
    return m_items.size();
  }

  virtual Item GetItem(size_t idx) const {
    return m_items.at(idx);
  }

  virtual size_t GatherersCount() const {
    return m_gatherers.size();
  }

  virtual Gatherer GetGatherer(size_t idx) const {
    return m_gatherers.at(idx);
  }

  std::vector<Item> m_items;
  std::vector<Gatherer> m_gatherers;
};

/**/
double randomDouble(double const & min, double const & max)
{
    static std::mt19937 gen;
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

std::pair<double,double> randMinMax(double const & min, double const & max) {
  const double val1 = randomDouble(min, max);
  const double val2 = randomDouble(min, max);
  return std::pair<double,double>( std::min(val1, val2), std::max(val1, val2) );
}

Point2D randomPoint(const Point2D& min, const Point2D& max ) {
  const double x = randomDouble(min.x, max.x);
  const double y = randomDouble(min.y, max.y);
  return Point2D(x, y);
}

ItemGatherer Init(size_t num, Point2D const& min, Point2D const& max, double const& maxMove = 50.0, double const& width = 0.6) {
  ItemGatherer ret;
  assert(num >= 2);
  for(size_t i = 0; i < num-1; ++i) {
    ret.m_items.emplace_back(randomPoint(min , max), width);
    Point2D start = randomPoint(min, max);
    Point2D end;
    if(i%2) {
      // horizontal
      end.x = start.x;
      end.y = randomDouble ( start.y - maxMove, start.y + maxMove);
      ret.m_gatherers.emplace_back(start, end, width);
    } else {
      end.y = start.y;
      end.x = randomDouble ( start.x - maxMove, start.x + maxMove);
      ret.m_gatherers.emplace_back(start, end, width);
    }
  }

  ret.m_items.emplace_back(Point2D( 0.0 , maxMove/2 ), width);
  ret.m_gatherers.emplace_back(Point2D(0.0,0.0), Point2D(0.0, maxMove), width);
  
  return ret;
}
/**/

auto compTime = [](const GatheringEvent& a, const GatheringEvent& b) {
  return a.time < b.time;
};


SCENARIO("Gather Events") {
  GIVEN("Random test") {

    const size_t num = GENERATE(5, 20, 100);
    const double maxMove = GENERATE(25.0, 50.0, 100.0);
    const double width = 0.6f;
    auto itemg = Init (num, Point2D(0.0, 0.0), Point2D(500.0, 500.0), maxMove, width);
    auto arr = FindGatherEvents(itemg);


    WHEN("Check val"){
      THEN("check sq_distance"){
        CHECK(arr.size()>=1);
        CHECK(std::is_sorted(arr.begin(), arr.end(), compTime));
      }
    }

    AND_WHEN("Check throw") {
        for(auto&i : arr) {
          REQUIRE_NOTHROW(itemg.GetGatherer(i.gatherer_id));
          REQUIRE_NOTHROW(itemg.GetItem(i.item_id));
        }
        // THEN("Check collect") {
        //   for(auto&i : arr) {
        //     auto gath = itemg.GetGatherer(i.gatherer_id);
        //     auto itm = itemg.GetItem(i.item_id);
        //     auto collect = TryCollectPoint(gath.start_pos, gath.end_pos, itm.position);
        //     CHECK(collect.IsCollected(width));
        //   }
        // }
    }
  }
}