//
// Created by chris on 2/8/20.
//

#ifndef CLASSINFO_H
#define CLASSINFO_H

#include <string>
#include <unordered_map>
#include <string>
#include <vector>
#include <map>
#include <ast/general/Type.h>
#include <util/macros.h>
#include "../entities/Entity.h"
#include "ConstFunction.h"

class InstanceMethod {
public:
    Path instance;
    std::unique_ptr<ConstFunction> func;

    InstanceMethod(Path instance, std::unique_ptr<ConstFunction> func) : instance(instance), func(std::move(func)) {
    }
};


enum class ClassAttributeType {
    member, static_member, method, static_method, not_found
};

class ConcreteClass {
public:
    std::vector<std::string> implemented_typeclasses;
    ConcreteClass(const std::string& class_name, Path path);

    VectorOfStrings member_names;
    std::vector<ast::Type*> member_types;
    std::unordered_map<std::string, std::unique_ptr<Entity>> member_entities;

    MapStringType members;
    std::unordered_map<std::string, std::unique_ptr<InstanceMethod>> methods;

    std::map<std::string, std::pair<ast::Type*, ast::ExpNode*>> static_members;
    std::unordered_map<std::string, std::unique_ptr<ConstFunction>> static_methods;

    const Path path;
    const std::string class_name;

    std::map<std::string, ClassAttributeType> all_attributes;

    ClassAttributeType get_attribute(const std::string& name) {
        auto attribute_type = this->all_attributes.find(name);
        if (attribute_type == this->all_attributes.end()) {
            return ClassAttributeType::not_found;
        }
        return attribute_type->second;
    }

    InstanceMethod& get_method(const std::string& name) {
        return *this->methods[name];
    }

    ConstFunction& get_static_method(const std::string& name) {
        return *this->static_methods[name];
    }

    Entity& get_member(const std::string& name) {
        return *this->member_entities[name];
    }

    ~ConcreteClass();

    bool operator!=(const ConcreteClass& b) const;

    bool operator==(const ConcreteClass& b) const;
    VectorOfStrings type_params;
};

#endif //CLASSINFO_H
