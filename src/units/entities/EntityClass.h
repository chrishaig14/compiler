//
// Created by chris on 25/9/21.
//

#ifndef XLANG_ENTITYCLASS_H
#define XLANG_ENTITYCLASS_H

#include "Entity.h"

class EntityClass : public Entity {
public:
    explicit EntityClass(Class& clazz) : Entity(E_TYPE::CLASS), clazz(clazz) {
    }

    bool equal(const Entity& other) const override {

        return &this->clazz == &((const EntityClass&) other).clazz;
    }

    Entity* clone() const override {
        return new EntityClass(this->clazz);
    }

    Class& clazz;

    EntityClass& get_class() override {
        return *this;
    }

    const EntityClass& get_class() const override {
        return *this;
    }
};

#endif //XLANG_ENTITYCLASS_H
