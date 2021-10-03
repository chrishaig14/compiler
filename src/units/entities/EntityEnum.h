//
// Created by chris on 25/9/21.
//

#ifndef XLANG_ENTITYENUM_H
#define XLANG_ENTITYENUM_H

#include "Entity.h"

class EntityEnum : public Entity {
public:
    explicit EntityEnum(Enum& enumm) : Entity(E_TYPE::ENUM), enumm(enumm) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }

    std::unique_ptr<Entity> clone() const override {
        return std::make_unique<EntityEnum>(this->enumm);
    }

    EntityEnum& get_enum() override {
        return *this;
    }

    const EntityEnum& get_enum() const override {
        return *this;
    }

    Enum& enumm;
};

#endif //XLANG_ENTITYENUM_H
