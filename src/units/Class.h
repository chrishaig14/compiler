//
// Created by chris on 2/8/20.
//

#ifndef CLASSINFO_H
#define CLASSINFO_H

#include <string>
#include <unordered_map>
#include <string>
#include <vector>
#include "../nodes/TypeNode.h"
#include "../macros.h"
#include "Entity.h"

class Class : public Entity {
public:
    VectorOfStrings member_names;
    std::vector<TypeNode*> member_types;

    MapStringType members;
    std::unordered_map<std::string, FunctionType*> methods;

    std::map<std::string, std::pair<TypeNode*, Node*>> static_members;
    std::unordered_map<std::string, FunctionType*> static_methods;

    std::string class_name;

    ~Class();

    bool operator!=(const Class& b) const;

    bool operator==(const Class& b) const;

    VectorOfStrings type_params;
    std::string full_path;
};

#endif //CLASSINFO_H
