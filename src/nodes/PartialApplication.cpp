//
// Created by chris on 7/12/20.
//

#include "PartialApplication.h"


bool PartialApplication::equal(const Node& other) const {
    const PartialApplication& other_p = (PartialApplication&)other;
    if ((*(this->function)) != (*(other_p.function))) {
        return false;
    }
    if (this->args.size() != other_p.args.size()) {
        return false;
    }
    for (size_t i = 0; i < this->args.size(); i++) {
        if (this->args[i] == nullptr && other_p.args[i] != nullptr) {
            return false;
        } else if (this->args[i] != nullptr && other_p.args[i] == nullptr) {
            return false;
        } else if (this->args[i] != nullptr && other_p.args[i] != nullptr) {
            if (*this->args[i] != *other_p.args[i]) {
                return false;
            }
        }
    }
    return true;
}

PartialApplication::PartialApplication(Node* function, VectorOfNodes args, TextPosition start, TextPosition end) : Node(
        NodeType::PARTIAL,
        start,
        end) {
    this->function = function;
    this->args = args;
}

nlohmann::json PartialApplication::to_json() {
    return nlohmann::json();
}

