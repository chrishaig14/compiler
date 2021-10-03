//
// Created by chris on 25/9/21.
//

#ifndef XLANG_ENTITYERROR_H
#define XLANG_ENTITYERROR_H

#include "Entity.h"

class EntityError : public Entity {
public:
    explicit EntityError() : Entity(E_TYPE::ERROR) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }

    std::unique_ptr<Entity> clone() const override {
        return std::make_unique<EntityError>();
    }

    EntityError& get_error() override {
        return *this;
    }

    const EntityError& get_error() const override {
        return *this;
    }
};


#endif //XLANG_ENTITYERROR_H
