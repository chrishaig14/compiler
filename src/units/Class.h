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
#include "../ast/Type.h"
#include "../macros.h"
#include "Entity.h"
#include "ConstFunction.h"

class Class {
public:
    Class(const std::string& class_name, Path path) : path(path), class_name(class_name) {
    }

    VectorOfStrings member_names;
    std::vector<ast::Type*> member_types;
    std::unordered_map<std::string, std::unique_ptr<Entity>> member_entities;

    MapStringType members;
    std::unordered_map<std::string, std::unique_ptr<ConstFunction>> methods;

    std::map<std::string, std::pair<ast::Type*, ast::Node*>> static_members;
    std::unordered_map<std::string, std::unique_ptr<ConstFunction>> static_methods;

    const Path path;
    const std::string class_name;

    ~Class();

    bool operator!=(const Class& b) const;

    bool operator==(const Class& b) const;
    VectorOfStrings type_params;
};

#endif //CLASSINFO_H
