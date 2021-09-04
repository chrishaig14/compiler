//
// Created by chris on 8/4/21.
//

#ifndef XLANG_OBJECTVALUE_H
#define XLANG_OBJECTVALUE_H


#include <cassert>
#include "Entity.h"
#include "../ast/TypeNode.h"

enum class Meta {
    ENUM, CLASS
};

class Value {

public:
    ast::TypeNode* type;
    union {
        Class* clazz;
        Enum* enumm;
    };
    Meta metatype;

    Value(ast::TypeNode* type) {
        this->clazz = nullptr;
        this->enumm = nullptr;
        assert(type != nullptr);
        this->type = type;
    }
};


class EntityValue : public Entity {
public:
    explicit EntityValue(std::unique_ptr<Value> value) : Entity(E_TYPE::VALUE), value(std::move(value)) {
    }

    std::unique_ptr<Value> value;

    bool equal(const Entity& other) const override {
        return false;
    }
};

#endif //XLANG_OBJECTVALUE_H
