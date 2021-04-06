//
// Created by chris on 1/8/20.
//

#include "IdNode.h"

IdNode::IdNode(std::string identifier, TextPosition start, TextPosition end) :Node(NodeType::ID, start, end), _id(identifier) {
    this->is_global_function = false;
}


bool IdNode::equal(const Node& x) const {
    auto& other = x.id();
    return this->_id == other._id;
}


IdNode& IdNode::id() {
    return *this;
}

const IdNode& IdNode::id() const {
    return *this;
}

json IdNode::to_json() const {
    json j;
    j["node"] = "id";
    j["id"] = this->_id;
    // j["start"] = this->start;
    // j["end"] = this->end;
    return j;
}
