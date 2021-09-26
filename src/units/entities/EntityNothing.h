//
// Created by chris on 25/9/21.
//

#ifndef XLANG_ENTITYNOTHING_H
#define XLANG_ENTITYNOTHING_H

#include "Entity.h"

class EntityNothing : public Entity {
public:
    explicit EntityNothing() : Entity(E_TYPE::NOTHING) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }


    Entity* clone() const override {
        return new EntityNothing();
    }

    EntityNothing& get_nothing() override {
        return *this;
    }

    const EntityNothing& get_nothing() const override {
        return *this;
    }
};

#endif //XLANG_ENTITYNOTHING_H
