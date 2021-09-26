//
// Created by chris on 8/4/21.
//

#ifndef XLANG_OBJECTVALUE_H
#define XLANG_OBJECTVALUE_H


#include <cassert>
#include "entities/Entity.h"
#include "../ast/general/Type.h"




// class EntityValue : public Entity {
// public:
//     explicit EntityValue(std::unique_ptr<Value> value) : Entity(E_TYPE::VALUE), value(std::move(value)) {
//     }
//
//     std::unique_ptr<Value> value;
//
//     bool equal(const Entity& other) const override {
//         return false;
//     }
//
//     Entity* clone() const override {
//         return new EntityValue(std::unique_ptr<Value>(this->value->clone()));
//     }
// };

#endif //XLANG_OBJECTVALUE_H
