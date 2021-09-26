//
// Created by chris on 25/9/21.
//

#ifndef XLANG_ENTITYNOTFOUND_H
#define XLANG_ENTITYNOTFOUND_H

#include "Entity.h"

class EntityNotFound : public Entity {
public:
    explicit EntityNotFound() : Entity(E_TYPE::NOT_FOUND) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }

    EntityNotFound* clone() const override {
        return new EntityNotFound();
    }

    EntityNotFound& get_notfound() override {
        return *this;
    }

    const EntityNotFound& get_notfound() const override {
        return *this;
    }
};

#endif //XLANG_ENTITYNOTFOUND_H
