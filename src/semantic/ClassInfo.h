//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_CLASSINFO_H
#define UNTITLED1_CLASSINFO_H

#include <string>
#include <map>
#include "ObjectInfo.h"
#include "FunctionInfo.h"
#include "SymbolInfo.h"

class ClassInfo {
public:
    std::vector<std::string> field_names;
    std::vector<TypeNode*> field_types;
    std::map<std::string, TypeNode*> fields;
    std::vector<std::string> type_parameters;

    ClassInfo() {}

    ClassInfo(const std::vector<std::string>& fieldNames, const std::vector<TypeNode*>& fieldTypes,
              std::vector<std::string> type_parameters) : field_names(
            fieldNames), field_types(fieldTypes), type_parameters(type_parameters) {
        for (int i = 0; i < fieldNames.size(); i++) {
            this->fields[fieldNames[i]] = fieldTypes[i];
        }
    }

    bool operator!=(const ClassInfo& b) const {
        return !(*this == b);
    }

    bool operator==(const ClassInfo& b) const {
        if (this->field_names != b.field_names) return false;
        if (this->field_types.size() != b.field_types.size()) return false;
        for (int i = 0; i < this->field_types.size(); i++) {
            if (!this->field_types[i]->equal(b.field_types[i])) return false;
        }
        return true;
    }

};

#endif //UNTITLED1_CLASSINFO_H
