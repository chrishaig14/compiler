//
// Created by chris on 2/8/20.
//

#ifndef CLASSINFO_H
#define CLASSINFO_H

#include <string>
#include <map>
#include "ObjectInfo.h"
#include "FunctionInfo.h"

class ClassInfo {
public:
    std::vector<std::string> member_names;
    VectorOfTypes member_types;

    std::map<std::string, TypeNode*> members;
    std::map<std::string, FunctionTypeNode*> methods;

    std::string class_name;

    ClassInfo() {}

    ClassInfo(std::string class_name, const std::vector<std::string>& fieldNames,
              const VectorOfTypes& fieldTypes, std::vector<std::string> type_parameters);

    bool operator!=(const ClassInfo& b) const;

    bool operator==(const ClassInfo& b) const;

    std::vector<std::string> type_parameters;
};

#endif //CLASSINFO_H
