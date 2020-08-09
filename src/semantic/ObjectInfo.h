//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_OBJECTINFO_H
#define UNTITLED1_OBJECTINFO_H

#include <string>
#include <vector>
#include "../nodes/TypeNode.h"

class ObjectInfo {
public:
    std::string parent;
    std::vector<ObjectInfo*> type_parameters;

    ObjectInfo(TypeNode* n) {
        this->parent = n->identifier;
        for (auto t: n->type_parameters) {
            this->type_parameters.push_back(new ObjectInfo(t));
        }
    }

    bool operator!=(const ObjectInfo& other) const {
        return !(*this == other);
    }

    bool operator==(const ObjectInfo& other) const {
        if (this->parent != other.parent) return false;
        if (this->type_parameters.size() != other.type_parameters.size()) return false;
        for (int i = 0; i < this->type_parameters.size(); i++) {
            if (*this->type_parameters[i] != *other.type_parameters[i]) return false;
        }
        return true;
    }

    std::string to_string() const {
        std::string templ;
        if (this->type_parameters.size() != 0) {
            for (int i = 0; i < this->type_parameters.size() - 1; i++) {
                templ += this->type_parameters[i]->to_string() + ",";
            }
            templ += this->type_parameters[this->type_parameters.size() - 1]->to_string();
        }
        return this->parent + templ;
    }
};

#endif //UNTITLED1_OBJECTINFO_H
