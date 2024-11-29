#include "model.h"

#include <algorithm>
#include <cmath>

namespace model {
    using namespace std::literals;

    double RoundToTwoDigits(double d) {
        return (static_cast<int>(d * 100 + 0.5) / 100.0);
    }

    void Map::AddRoad(const Road& road) {
        roads_.emplace_back(road);
        std::shared_ptr<Road> road_ptr = std::make_shared<Road>(roads_.back());

        if (road.IsHorizontal()) {
            for (Coord x = std::min(road_ptr->GetStart().x, road_ptr->GetEnd().x); x <= std::max(road_ptr->GetStart().x, road_ptr->GetEnd().x); x++) {
                coord_to_road[{x, road_ptr->GetStart().y}].emplace(road_ptr);
            }
        }
        if (road.IsVertical()) {
            for (Coord y = std::min(road_ptr->GetStart().y, road_ptr->GetEnd().y); y <= std::max(road_ptr->GetStart().y, road_ptr->GetEnd().y); y++) {
                coord_to_road[{road_ptr->GetStart().x, y}].emplace(road_ptr);
            }
        }
    }

    void Map::AddOffice(Office office) {
        if (warehouse_id_to_index_.contains(office.GetId())) {
            throw std::invalid_argument("Duplicate warehouse");
        }
        const size_t index = offices_.size();
        Office& o = offices_.emplace_back(std::move(office));
        try {
            warehouse_id_to_index_.emplace(o.GetId(), index);
        }
        catch (...) {
            // Удаляем офис из вектора, если не удалось вставить в unordered_map
            offices_.pop_back();
            throw;
        }
    }

    std::set<std::shared_ptr<Road>> Map::GetRoadsByCoords(Point p) const {
        if (coord_to_road.contains(p)) {
            return coord_to_road.at(p);
        }
        return {};
    }

    ParamPairDouble Map::GetRandomDogPosition() const {
        if (roads_.empty()) {
            throw std::runtime_error("No roads to put dog on...");
        }

        std::random_device rd;
        std::mt19937 gen{ rd() };
        int road_number = 0;
        if (roads_.size() > 1) {
            std::uniform_int_distribution<int> dist(0, roads_.size() - 1);
            road_number = dist(gen);
        }
        Road road = roads_.at(road_number);
        Point road_start = road.GetStart();
        Point road_end = road.GetEnd();

        ParamPairDouble random_point{ static_cast<double>(road_start.x), static_cast<double>(road_start.y) };
        double width;
        {
            std::uniform_real_distribution<> dist(-0.4, 0.4);
            width = dist(gen);
            width = RoundToTwoDigits(width);
        }

        if (road.IsHorizontal()) {
            std::uniform_real_distribution<> dist(road_start.x, road_end.x);
            random_point.x_ = dist(gen);
            random_point.y_ += width;
        }
        if (road.IsVertical()) {
            std::uniform_real_distribution<> dist(road_start.y, road_end.y);
            random_point.y_ = dist(gen);
            random_point.x_ += width;
        }
        random_point.x_ = RoundToTwoDigits(random_point.x_);
        random_point.y_ = RoundToTwoDigits(random_point.y_);

        return random_point;
    }

    ParamPairDouble Map::GetStartPosition(bool random) const {
        if (random) {
            return GetRandomDogPosition();
        }
        Point p = roads_.at(0).GetStart();
        return { p.x * 1., p.y * 1. };
    }

    void Map::PrintCoordToRoad() const {
        for (auto [point, road] : coord_to_road) {

            std::cout << "{" << point.x << ", " << point.y << "} " << ": " << std::endl;
            for (auto r : road) {
                std::cout << "road ptr " << r << std::endl;
            }
        }
    }

    void Element::SetKeySequence(std::string str) {
        keys_.push_back(str);
    }

    std::vector<std::string> Element::GetKeys() const {
        return keys_;
    }

    bool Road::IsHorizontal() const noexcept {
        return start_.y == end_.y;
    }

    bool Road::IsVertical() const noexcept {
        return start_.x == end_.x;
    }

    Point Road::GetStart() const noexcept {
        return start_;
    }

    Point Road::GetEnd() const noexcept {
        return end_;
    }

    bool Road::PointIsOnRoad(ParamPairDouble& point) const {
        return ((point.x_ >= road_area_.left_bottom.x_ && point.x_ <= road_area_.right_top.x_) &&
            (point.y_ >= road_area_.left_bottom.y_ && point.y_ <= road_area_.right_top.y_));
    }

    RoadArea Road::GetRoadArea() const {
        return road_area_;
    }

    void Road::SetRoadArea() {
        if (this->IsHorizontal()) {
            road_area_.left_bottom = { std::min(start_.x, end_.x) - 0.4, start_.y - 0.4 };
            road_area_.right_top = { std::max(start_.x, end_.x) + 0.4, start_.y + 0.4 };
        }
        if (this->IsVertical()) {
            road_area_.left_bottom = { start_.x - 0.4, std::min(start_.y, end_.y) - 0.4 };
            road_area_.right_top = { start_.x + 0.4, std::max(start_.y, end_.y) + 0.4 };
        }
    }

    const Rectangle& Building::GetBounds() const noexcept {
        return bounds_;
    }

    const Office::Id& Office::GetId() const noexcept {
        return id_;
    }

    Point Office::GetPosition() const noexcept {
        return position_;
    }

    Offset Office::GetOffset() const noexcept {
        return offset_;
    }

    const Map::Id& Map::GetId() const noexcept {
        return id_;
    }

    const std::string& Map::GetName() const noexcept {
        return name_;
    }

    const Map::Buildings& Map::GetBuildings() const noexcept {
        return buildings_;
    }

    const Map::Roads& Map::GetRoads() const noexcept {
        return roads_;
    }

    const Map::Offices& Map::GetOffices() const noexcept {
        return offices_;
    }

    void Map::AddBuilding(const Building& building) {
        buildings_.emplace_back(building);
    }

    void Map::SetMapDogSpeed(double ds) {
        map_dog_speed_ = ds;
    }

    double Map::GetMapDogSpeed() const {
        return map_dog_speed_;
    }

    void Map::AddLootTypes(const std::vector<LootType>& lootTypes) {
        lootTypes_ = lootTypes;
    }

    void Map::GenerateLoot(std::chrono::milliseconds time_delta) {
        unsigned lootCount = lostObjects_.size();
        unsigned looterCount = dogs_.size(); // Используем список собак на карте

        unsigned newLootCount = lootGenerator_.Generate(time_delta, lootCount, looterCount);

        std::uniform_int_distribution<int> typeDist(0, lootTypes_.size() - 1);
        std::uniform_int_distribution<int> roadDist(0, roads_.size() - 1);

        for (unsigned i = 0; i < newLootCount; ++i) {
            int type = typeDist(randomGenerator_);
            int roadIndex = roadDist(randomGenerator_);
            const Road& road = roads_[roadIndex];

            Point pos;
            if (road.IsHorizontal()) {
                std::uniform_int_distribution<int> xDist(road.GetStart().x, road.GetEnd().x);
                pos.x = xDist(randomGenerator_);
                pos.y = road.GetStart().y;
            }
            else {
                std::uniform_int_distribution<int> yDist(road.GetStart().y, road.GetEnd().y);
                pos.x = road.GetStart().x;
                pos.y = yDist(randomGenerator_);
            }

            lostObjects_[nextLootId_++] = { type, pos };
        }
    }

    const std::unordered_map<int, std::pair<int, Point>>& Map::GetLostObjects() const {
        return lostObjects_;
    }

    const std::vector<LootType>& Map::GetLootTypes() const {
        return lootTypes_;
    }

}