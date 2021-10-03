//
// Created by chris on 25/9/21.
//

#ifndef XLANG_ENTITYMODULE_H
#define XLANG_ENTITYMODULE_H

#include "Entity.h"

class EntityModule : public Entity {
public:
    explicit EntityModule(Module& module) : Entity(E_TYPE::MODULE), module(module) {
    }

    bool equal(const Entity& other) const override {
        return false;
    }

    std::unique_ptr<Entity> clone() const override {
        return std::make_unique<EntityModule>(this->module);
    }

    Module& module;

    EntityModule& get_module() override {
        return *this;
    }

    const EntityModule& get_module() const override {
        return *this;
    }
};

#endif //XLANG_ENTITYMODULE_H
