//
// Created by chris on 1/8/20.
//

#include "CallNode.h"

CallNode::CallNode(Node* function, const VectorOfNodes& arguments, TextPosition start, TextPosition end) : Node(NodeType::CALL,
                                                                                                                start,
                                                                                                                end),
                                                                                                           function(
                                                                                                                   function),
                                                                                                           arguments(
                                                                                                                   arguments) {
}

bool CallNode::equal(const Node& x) const {
    auto& other = x.call();
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

CallNode& CallNode::call() {
    return *this;
}

const CallNode& CallNode::call() const {
    return *this;
}

json CallNode::to_json() const {
    json j;
    j["node"] = "call";
//        j["function"] = this->function->to_json();
    j["arguments"] = {};
    for (auto a: this->arguments) {
//            j["arguments"].push_back(a->to_json());
    }
    return j;
}

CallNode::~CallNode() {
    delete this->function;
    for (auto a: this->arguments) {
        delete a;
    }
    for (auto at: this->arg_types) {
        delete at;
    }
    for (auto ft: this->ftype) {
        delete ft;
    }
}
