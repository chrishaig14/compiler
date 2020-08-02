//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_FUNCTIONINFO_H
#define UNTITLED1_FUNCTIONINFO_H

#include "../nodes/TypeNode.h"
#include "ObjectInfo.h"

class FunctionInfo {
public:
    std::vector<ObjectInfo*> parameter_types;
    ObjectInfo* return_type;

    FunctionInfo(std::vector<TypeNode*> parameter_types, TypeNode* return_type) {
        for (auto n: parameter_types) {
            this->parameter_types.push_back(new ObjectInfo(n));
        }
        this->return_type = new ObjectInfo(return_type);
    }

    bool operator!=(const FunctionInfo& b) const {
        return !(*this == b);
    }

    bool operator==(const FunctionInfo& b) const {
        if (*this->return_type != *b.return_type) return false;
        if (this->parameter_types.size() != b.parameter_types.size()) return false;
        for (int i = 0; i < this->parameter_types.size(); i++) {
            if (*this->parameter_types[i] != *b.parameter_types[i])return false;
        }
        return true;
    }

};

#endif //UNTITLED1_FUNCTIONINFO_H
