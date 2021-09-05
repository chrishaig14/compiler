//
// Created by chris on 8/4/21.
//

#ifndef XLANG_OBJECTVALUE_H
#define XLANG_OBJECTVALUE_H


#include <cassert>
#include "Entity.h"
#include "../ast/Type.h"

enum class Meta {
    ENUM, CLASS
};

class Value {

public:
    ast::Type* type;
    union {
        Class* clazz;
        Enum* enumm;
    };
    Meta metatype;

    Value(ast::Type* type) {
        this->clazz = nullptr;
        this->enumm = nullptr;
        assert(type != nullptr);
        this->type = type;
    }

    Value* clone() const {
        auto* v = new Value(type);
        v->metatype = this->metatype;
        switch (this->metatype) {
            case Meta::CLASS: {
                v->clazz = this->clazz;
                break;
            }
            case Meta::ENUM: {
                v->enumm = this->enumm;
                break;
            }
        }
        return v;
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

    Entity* clone() const override {
        return new EntityValue(std::unique_ptr<Value>(this->value->clone()));
    }
};

#endif //XLANG_OBJECTVALUE_H
