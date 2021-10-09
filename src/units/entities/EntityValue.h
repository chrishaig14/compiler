//
// Created by chris on 25/9/21.
//

#ifndef XLANG_ENTITYVALUE_H
#define XLANG_ENTITYVALUE_H

#include "Entity.h"
#include "../../simple_nodes/sem.h"
#include "../../simple_nodes/common/include/Type.h"
#include <cassert>

enum class Meta {
    ENUM, CLASS, FUNCTION
};

class EntityValue : public Entity {
    sem::UType _type;
private:
    EntityValue(sem::Type* type, Meta meta);

public:
    sem::Type& type;
    union {
        ConcreteClass* clazz;
        Enum* enumm;
    };
    const Meta metatype;


    EntityValue(sem::Type* type, ConcreteClass* cls);

    EntityValue(sem::Type* type, Enum* enumm);

    static std::unique_ptr<EntityValue> function_value(sem::Type* type);

    std::unique_ptr<Entity> clone() const override;

    bool equal(const Entity& other) const override;

    EntityValue& get_value() override;

    const EntityValue& get_value() const override;
};

#endif //XLANG_ENTITYVALUE_H
