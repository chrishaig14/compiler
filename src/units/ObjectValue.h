//
// Created by chris on 8/4/21.
//

#ifndef XLANG_OBJECTVALUE_H
#define XLANG_OBJECTVALUE_H


#include <cassert>
#include "Entity.h"
#include "../ast/general/Type.h"

enum class Meta {
    ENUM, CLASS
};

class EntityValue : public Entity {
    sem::UType _type;
public:
    sem::Type& type;
    union {
        Class* clazz;
        Enum* enumm;
    };
    Meta metatype;

    EntityValue(sem::Type* type) : Entity(E_TYPE::VALUE), type(*type) {
        this->clazz = nullptr;
        this->enumm = nullptr;
        assert(type != nullptr);
        this->_type = sem::UType(type);
    }

    EntityValue(sem::Type* type, Class* cls) : Entity(E_TYPE::VALUE), type(*type) {
        this->clazz = cls;
        assert(type != nullptr);
        this->_type = sem::UType(type);
        this->metatype = Meta::CLASS;
    }

    Entity* clone() const override {
        auto* v = new EntityValue(this->type.clone());
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

    bool equal(const Entity& other) const override {
        auto& o = (const EntityValue&) other;
        return this->metatype == o.metatype && this->type == o.type && this->clazz == o.clazz;
    }

    EntityValue& get_value() override {
        return *this;
    }

    const EntityValue& get_value() const override {
        return *this;
    }
};


// class EntityValue : public Entity {
// public:
//     explicit EntityValue(std::unique_ptr<Value> value) : Entity(E_TYPE::VALUE), value(std::move(value)) {
//     }
//
//     std::unique_ptr<Value> value;
//
//     bool equal(const Entity& other) const override {
//         return false;
//     }
//
//     Entity* clone() const override {
//         return new EntityValue(std::unique_ptr<Value>(this->value->clone()));
//     }
// };

#endif //XLANG_OBJECTVALUE_H
