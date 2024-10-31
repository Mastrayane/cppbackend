#include "visitor.h"
#include "model.h"  

namespace http_handler {

    void JsonVisitor::Visit(const model::Road& road) {
        if (road.IsHorizontal()) {
            mapJson_.at("roads").as_array().emplace_back(boost::json::object{
                {"x0", road.GetStart().x},
                {"y0", road.GetStart().y},
                {"x1", road.GetEnd().x}
                });
        }
        else if (road.IsVertical()) {
            mapJson_.at("roads").as_array().emplace_back(boost::json::object{
                {"x0", road.GetStart().x},
                {"y0", road.GetStart().y},
                {"y1", road.GetEnd().y}
                });
        }
    }

    void JsonVisitor::Visit(const model::Building& building) {
        const auto& bounds = building.GetBounds();
        mapJson_.at("buildings").as_array().emplace_back(boost::json::object{
            {"x", bounds.position.x},
            {"y", bounds.position.y},
            {"w", bounds.size.width},
            {"h", bounds.size.height}
            });
    }

    void JsonVisitor::Visit(const model::Office& office) {
        mapJson_.at("offices").as_array().emplace_back(boost::json::object{
            {"id", *office.GetId()},
            {"x", office.GetPosition().x},
            {"y", office.GetPosition().y},
            {"offsetX", office.GetOffset().dx},
            {"offsetY", office.GetOffset().dy}
            });
    }

}  // namespace http_handler