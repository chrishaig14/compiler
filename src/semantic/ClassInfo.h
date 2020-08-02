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
    std::map<std::string, SymbolInfo*> fields;
    std::map<std::string, FunctionInfo*> methods;

    ClassInfo(std::map<std::string, SymbolInfo*> fields,
              std::map<std::string, FunctionInfo*> methods) {
        this->fields = fields;
        this->methods = methods;
    }

    ClassInfo() {}

    bool operator!=(const ClassInfo& b) const {
        return !(*this == b);
    }

    bool operator==(const ClassInfo& b) const {
        if (this->methods.size() != b.methods.size()) return false;
        for (auto it : this->methods) {
            if (b.methods.count(it.first) == 0)return false;
            if (*(it.second) != *(b.methods.at(it.first))) return false;
        }
        if (this->fields.size() != b.fields.size()) return false;
        for (auto it : this->fields) {
            if (b.fields.count(it.first) == 0)return false;
            if (*(it.second) != *(b.fields.at(it.first))) return false;
        }
        return true;
    }

};

#endif //UNTITLED1_CLASSINFO_H
