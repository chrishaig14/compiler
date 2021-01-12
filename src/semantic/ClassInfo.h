//
// Created by chris on 2/8/20.
//

#ifndef CLASSINFO_H
#define CLASSINFO_H

#include <string>
#include <unordered_map>
#include "ObjectInfo.h"
#include "FunctionInfo.h"
#include "../macros.h"

class ClassInfo {
public:
    std::vector<std::string> member_names;
    VectorOfTypes member_types;

    MapStringType members;
    std::unordered_map<std::string, FunctionType*> methods;

    std::string class_name;

    ClassInfo() {}

    ClassInfo(std::string class_name, const std::vector<std::string>& fieldNames,
              const VectorOfTypes& fieldTypes, std::vector<std::string> type_parameters);

    ~ClassInfo() {
        for (auto m: this->members) {
            delete m.second;
        }
        for (auto m: this->methods) {
            delete m.second;
        }
        for (auto m: this->member_types) {
            delete m;
        }
    }

    bool operator!=(const ClassInfo& b) const;

    bool operator==(const ClassInfo& b) const;

    std::vector<std::string> type_parameters;
};

#endif //CLASSINFO_H
