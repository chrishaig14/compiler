//
// Created by chris on 25/9/21.
//

#include "EntityValue.h"

EntityValue::EntityValue(sem::Type* type, const ConcreteClass* cls) : Entity(E_TYPE::VALUE), type(*type), metatype(Meta::CLASS) {
    this->clazz = cls;
    assert(type != nullptr);
    this->_type = sem::UType(type);
}

EntityValue::EntityValue(sem::Type* type, const Enum* enumm) : Entity(E_TYPE::VALUE), type(*type), metatype(Meta::ENUM) {
    this->enumm = enumm;
    assert(type != nullptr);
    this->_type = sem::UType(type);
}

std::unique_ptr<EntityValue> EntityValue::function_value(sem::Type* type) {
    return std::unique_ptr<EntityValue>(new EntityValue(type, Meta::FUNCTION));
}

std::unique_ptr<Entity> EntityValue::clone() const {
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

bool EntityValue::equal(const Entity& other) const {
    auto& o = (const EntityValue&) other;
    return this->metatype == o.metatype && this->type == o.type && this->clazz == o.clazz;
}

EntityValue& EntityValue::get_value() {
    return *this;
}

const EntityValue& EntityValue::get_value() const {
    return *this;
}

EntityValue::EntityValue(sem::Type* type, Meta meta) : Entity(E_TYPE::VALUE), type(*type),metatype(meta) {
    this->clazz = nullptr;
    this->_type = sem::UType(type);
}
