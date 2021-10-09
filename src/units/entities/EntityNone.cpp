//
// Created by chris on 8/10/21.
//

#include "EntityNone.h"

std::unique_ptr<Entity> EntityNone::clone() const {
    return std::make_unique<EntityNone>();
}

bool EntityNone::equal(const Entity& other) const {
    return true;
}
