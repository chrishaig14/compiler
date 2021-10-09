//
// Created by chris on 8/10/21.
//

#ifndef XLANG_ENTITYNONE_H
#define XLANG_ENTITYNONE_H


#include "Entity.h"

class EntityNone : public Entity {
public:
    std::unique_ptr<Entity> clone() const override;
    bool equal(const Entity& other) const override;

    explicit EntityNone() : Entity(E_TYPE::NONE) {
    }
};


#endif //XLANG_ENTITYNONE_H
