#pragma once

#include "model.h"
#include <boost/json.hpp>

namespace http_handler {

    class ISerializer {
    public:
        virtual void Serialize(const model::Road& road) = 0;
        virtual void Serialize(const model::Building& building) = 0;
        virtual void Serialize(const model::Office& office) = 0;
    };

    class JsonSerializer : public ISerializer {
    public:
        explicit JsonSerializer(boost::json::object& serializedMap) : serializedMap_(serializedMap) {}

        void Serialize(const model::Road& road) override;
        void Serialize(const model::Building& building) override;
        void Serialize(const model::Office& office) override;

    private:
        boost::json::object& serializedMap_;
    };

}  // namespace http_handler