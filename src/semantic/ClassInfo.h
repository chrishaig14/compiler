//
// Created by chris on 2/8/20.
//

#ifndef CLASSINFO_H
#define CLASSINFO_H

#include <string>
#include <map>
#include "ObjectInfo.h"
#include "FunctionInfo.h"
#include "SymbolInfo.h"

class ClassInfo {
public:
    std::vector<std::string> member_names;
    std::vector<TypeNode> member_types;

    std::map<std::string, TypeNode> members;
    std::map<std::string, FunctionTypeNode> methods;

    std::string class_name;

    ClassInfo() {}

    ClassInfo(std::string class_name, const std::vector<std::string>& fieldNames,
              const std::vector<TypeNode>& fieldTypes, std::vector<std::string> type_parameters) : member_names(
            fieldNames), member_types(fieldTypes) {
        this->class_name = class_name;
        for (int i = 0; i < fieldNames.size(); i++) {
            this->members[fieldNames[i]] = fieldTypes[i];
        }
        for (int i = 0; i < fieldNames.size(); i++) {
            this->members[fieldNames[i]] = fieldTypes[i];
        }
        this->type_parameters = type_parameters;
    }

    bool operator!=(const ClassInfo& b) const {
        return !(*this == b);
    }

    bool operator==(const ClassInfo& b) const {
        if (this->member_names != b.member_names) return false;
        if (this->member_types.size() != b.member_types.size()) return false;

        for (int i = 0; i < this->member_types.size(); i++) {
            if (this->member_types[i] != b.member_types[i]) return false;
        }
        return true;
    }

    std::vector<std::string> type_parameters;
};

#endif //CLASSINFO_H
