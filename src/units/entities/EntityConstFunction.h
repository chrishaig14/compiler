//
// Created by chris on 25/9/21.
//

#ifndef XLANG_ENTITYCONSTFUNCTION_H
#define XLANG_ENTITYCONSTFUNCTION_H

#include "Entity.h"

class EntityConstFunction : public Entity {
public:
    explicit EntityConstFunction(ConstFunction& const_function)
    : Entity(E_TYPE::CONST_FUNCTION), const_function(const_function) {
    }

    ConstFunction& const_function;

    bool equal(const Entity& other) const override {
        return false;
    }

    Entity* clone() const override {
        return new EntityConstFunction(this->const_function);
    }

    EntityConstFunction& get_constfun() override {
        return *this;
    }

    const EntityConstFunction& get_constfun() const override {
        return *this;
    }
};


#endif //XLANG_ENTITYCONSTFUNCTION_H
