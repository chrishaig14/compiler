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
#include "../nodes/TypeNode.h"
#include "../macros.h"
#include "Entity.h"

class Class {
public:
    Class();
    VectorOfStrings member_names;
    std::vector<TypeNode*> member_types;

    MapStringType members;
    std::unordered_map<std::string, ConstFunction*> methods;

    std::map<std::string, std::pair<TypeNode*, Node*>> static_members;
    std::unordered_map<std::string, ConstFunction*> static_methods;

    std::string class_name;

    ~Class();

    bool operator!=(const Class& b) const;

    bool operator==(const Class& b) const;

    VectorOfStrings type_params;
    Path path;
};

#endif //CLASSINFO_H
