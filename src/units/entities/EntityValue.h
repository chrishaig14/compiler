//
// Created by chris on 25/9/21.
//

#ifndef XLANG_ENTITYVALUE_H
#define XLANG_ENTITYVALUE_H

#include "Entity.h"
#include "../../simple_nodes/sem.h"
#include "../../simple_nodes/common/include/Type.h"
#include <cassert>

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

    EntityValue(sem::Type* type, Class* cls) : Entity(E_TYPE::VALUE), type(*type), metatype(Meta::CLASS) {
        this->clazz = cls;
        assert(type != nullptr);
        this->_type = sem::UType(type);
    }

    EntityValue(sem::Type* type, Enum* enumm) : Entity(E_TYPE::VALUE), type(*type), metatype(Meta::ENUM) {
        this->enumm = enumm;
        assert(type != nullptr);
        this->_type = sem::UType(type);
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

#endif //XLANG_ENTITYVALUE_H
