//
// Created by chris on 25/9/21.
//

#ifndef XLANG_ENTITYPACKAGE_H
#define XLANG_ENTITYPACKAGE_H

#include "Entity.h"

class EntityPackage : public Entity {
public:
    explicit EntityPackage(Package& package) : Entity(E_TYPE::PACKAGE), package(package) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }

    Entity* clone() const override {
        return new EntityPackage(this->package);
    }

    Package& package;

    EntityPackage& get_package() override {
        return *this;
    }

    const EntityPackage& get_package() const override {
        return *this;
    }
};

#endif //XLANG_ENTITYPACKAGE_H
