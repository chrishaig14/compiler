//
// Created by chris on 1/8/20.
//

#include "FunctionNode.h"
#include "../utils.h"
void FunctionNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

bool FunctionNode::operator!=(const FunctionNode& other) const {
    return !(*this == other);
}

bool FunctionNode::operator==(const FunctionNode& other) const {
//    return false;
        if (this->identifier != other.identifier) return false;
        if (this->parameter_types.size() != other.parameter_types.size()) return false;
        if (this->parameter_names.size() != other.parameter_names.size()) return false;
        for (int i = 0; i < this->parameter_types.size(); i++) {
            if (this->parameter_types[i] != other.parameter_types[i]) return false;
        }
        for (int i = 0; i < this->parameter_names.size(); i++) {
            if (this->parameter_names[i] != other.parameter_names[i]) return false;
        }
        if (this->body != other.body) return false;
        return true;
}
