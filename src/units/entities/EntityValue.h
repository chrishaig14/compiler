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
    ENUM, CLASS, FUNCTION
};

class EntityValue : public Entity {
    sem::UType _type;
private:
    EntityValue(sem::Type* type, Meta meta) : Entity(E_TYPE::VALUE), type(*type),metatype(meta) {
        this->clazz = nullptr;
        this->_type = sem::UType(type);
    }

public:
    sem::Type& type;
    union {
        Class* clazz;
        Enum* enumm;
    };
    const Meta metatype;


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

    static std::unique_ptr<EntityValue> function_value(sem::Type* type) {
        return std::unique_ptr<EntityValue>(new EntityValue(type, Meta::FUNCTION));
    }

    std::unique_ptr<Entity> clone() const override {
        switch (this->metatype) {
            case Meta::CLASS: {
                return std::make_unique<EntityValue>(this->type.clone(), this->clazz);
            }
            case Meta::ENUM: {
                return std::make_unique<EntityValue>(this->type.clone(), this->enumm);
            }
            case Meta::FUNCTION:
                return EntityValue::function_value(this->type.clone());
        }
        return nullptr;
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
