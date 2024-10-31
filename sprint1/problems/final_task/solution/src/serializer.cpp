#include "serializer.h"
#include "model.h"  

namespace http_handler {

    void JsonSerializer::Serialize(const model::Road& road) {
        if (road.IsHorizontal()) {
            serializedMap_.at("roads").as_array().emplace_back(boost::json::object{
                {"x0", road.GetStart().x},
                {"y0", road.GetStart().y},
                {"x1", road.GetEnd().x}
                });
        }
        else if (road.IsVertical()) {
            serializedMap_.at("roads").as_array().emplace_back(boost::json::object{
                {"x0", road.GetStart().x},
                {"y0", road.GetStart().y},
                {"y1", road.GetEnd().y}
                });
        }
    }

    void JsonSerializer::Serialize(const model::Building& building) {
        const auto& bounds = building.GetBounds();
        serializedMap_.at("buildings").as_array().emplace_back(boost::json::object{
            {"x", bounds.position.x},
            {"y", bounds.position.y},
            {"w", bounds.size.width},
            {"h", bounds.size.height}
            });
    }

    void JsonSerializer::Serialize(const model::Office& office) {
        serializedMap_.at("offices").as_array().emplace_back(boost::json::object{
            {"id", *office.GetId()},
            {"x", office.GetPosition().x},
            {"y", office.GetPosition().y},
            {"offsetX", office.GetOffset().dx},
            {"offsetY", office.GetOffset().dy}
            });
    }

}  // namespace http_handler