//
// Created by chris on 1/8/20.
//

#include "CallNode.h"
#include "../json/json.hpp"
using namespace ast;
CallNode::CallNode(UNode& function, VectorOfNodesU& arguments, TextPosition start, TextPosition end) : Node(
        NodeType::CALL,
        start,
        end), function(std::move(function)), arguments(std::move(arguments)) {
}

bool CallNode::equal(const Node& x) const {
    const auto& other = (CallNode&) x;
    if (this->arguments.size() != other.arguments.size()) {
        return false;
    }
    for (size_t i = 0; i < this->arguments.size(); ++i) {
        if (*this->arguments[i] != *other.arguments[i]) {
            return false;
        }
    }
    return *this->function == *other.function;
}

CallNode::~CallNode() {
    // delete this->function;
    // for (auto* a: this->arguments) {
    //     delete a;
    // }
    // for (auto* at: this->arg_types) {
    //     delete at;
    // }
    // for (auto* ft: this->ftype) {
    //     delete ft;
    // }
}

nlohmann::json CallNode::to_json() const {
    nlohmann::json j;
    j["type"] = "call";
    j["call"]["function"] = this->function->to_json();
    std::vector<nlohmann::json> v;
    for (auto& e: this->arguments) {
        v.push_back(e->to_json());
    }
    j["call"]["arguments"] = v;
    return j;
}
