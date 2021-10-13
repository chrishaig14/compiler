//
// Created by chris on 12/10/21.
//

#ifndef XLANG_ENTITYTYPECLASS_H
#define XLANG_ENTITYTYPECLASS_H

#include "Entity.h"

class EntityTypeclass : public Entity {
public:
    explicit EntityTypeclass(TypeclassFoo& clazz) : Entity(E_TYPE::TYPECLASS), clazz(clazz) {
    }

    bool equal(const Entity& other) const override {

        return &this->clazz == &((const EntityTypeclass&) other).clazz;
    }

    std::unique_ptr<Entity> clone() const override {
        return std::make_unique<EntityTypeclass>(this->clazz);
    }

    TypeclassFoo& clazz;

    EntityTypeclass& get_typeclass() override {
        return *this;
    }

    const EntityTypeclass& get_typeclass() const override {
        return *this;
    }
};

#endif //XLANG_ENTITYTYPECLASS_H
