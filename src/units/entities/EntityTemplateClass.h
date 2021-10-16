//
// Created by chris on 25/9/21.
//

#ifndef XLANG_ENTITYTEMPLATECLASS_H
#define XLANG_ENTITYTEMPLATECLASS_H

#include "Entity.h"

class EntityTemplateClass : public Entity {
public:
    explicit EntityTemplateClass(TemplateClassInfo& clazz) : Entity(E_TYPE::TEMPLATE_CLASS), clazz(clazz) {
    }

    bool equal(const Entity& other) const override {

        return &this->clazz == &((const EntityTemplateClass&) other).clazz;
    }

    std::unique_ptr<Entity> clone() const override {
        return std::make_unique<EntityTemplateClass>(this->clazz);
    }

    TemplateClassInfo& clazz;

    EntityTemplateClass& get_template_class() override {
        return *this;
    }

    const EntityTemplateClass& get_template_class() const override {
        return *this;
    }
};

#endif //XLANG_ENTITYTEMPLATECLASS_H
