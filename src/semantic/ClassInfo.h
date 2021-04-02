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

class ClassInfo {
public:
    VectorOfStrings member_names;
    VectorOfTypes member_types;

    MapStringType members;
    std::unordered_map<std::string, FunctionType*> methods;

    MapStringType static_members;
    std::unordered_map<std::string, FunctionType*> static_methods;

    std::string class_name;

    ClassInfo() {

    }

    ClassInfo(std::string class_name, const VectorOfStrings& fieldNames, const VectorOfTypes& fieldTypes,
              VectorOfStrings type_parameters);

    ~ClassInfo();

    bool operator!=(const ClassInfo& b) const;

    bool operator==(const ClassInfo& b) const;

    VectorOfStrings type_params;
};

#endif //CLASSINFO_H
