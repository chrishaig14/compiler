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


enum class ClassMemberCategory {
    attribute, static_attribute, method, static_method, not_found
};

class ConcreteClass {
public:
    std::vector<std::string> implemented_typeclasses;
    ConcreteClass(const std::string& class_name, Path path);

    VectorOfStrings attribute_names;
    std::vector<ast::Type*> attribute_types;
    std::unordered_map<std::string, std::unique_ptr<Entity>> attribute_entities;

    MapStringType attributes;
    std::unordered_map<std::string, std::unique_ptr<InstanceMethod>> methods;

    std::map<std::string, std::pair<ast::Type*, ast::ExpNode*>> static_attributes;
    std::unordered_map<std::string, std::unique_ptr<InstanceMethod>> static_methods;

    const Path path;
    const std::string class_name;

    std::map<std::string, ClassMemberCategory> all_members;

    ClassMemberCategory get_member(const std::string& name) const {
        auto member_cat = this->all_members.find(name);
        if (member_cat == this->all_members.end()) {
            return ClassMemberCategory::not_found;
        }
        return member_cat->second;
    }

    InstanceMethod& get_method(const std::string& name) {
        return *this->methods[name];
    }

    InstanceMethod& get_static_method(const std::string& name) {
        return *this->static_methods[name];
    }

    Entity& get_attribute(const std::string& name) {
        return *this->attribute_entities[name];
    }

    ~ConcreteClass();

    bool operator!=(const ConcreteClass& b) const;

    bool operator==(const ConcreteClass& b) const;
    VectorOfStrings type_params;
};

#endif //CLASSINFO_H
