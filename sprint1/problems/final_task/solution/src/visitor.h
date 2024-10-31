#pragma once

#include "model.h"
#include <boost/json.hpp>

namespace http_handler {

class IVisitor {
public:
    virtual void Visit(const model::Road& road) = 0;
    virtual void Visit(const model::Building& building) = 0;
    virtual void Visit(const model::Office& office) = 0;
    // Добавьте другие методы для других типов объектов
};

class JsonVisitor : public IVisitor {
public:
    explicit JsonVisitor(boost::json::object& mapJson) : mapJson_(mapJson) {}

    void Visit(const model::Road& road) override;
    void Visit(const model::Building& building) override;
    void Visit(const model::Office& office) override;

private:
    boost::json::object& mapJson_;
};

}  // namespace http_handler