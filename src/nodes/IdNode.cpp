//
// Created by chris on 1/8/20.
//

#include "IdNode.h"
#include "../json/json.hpp"
IdNode::IdNode(std::string identifier, TextPosition start, TextPosition end) :Node(NodeType::ID, start, end), _id(identifier) {
    this->is_global_function = false;
}


bool IdNode::equal(const Node& x) const {
    const auto& other = (IdNode&)x;
    return this->_id == other._id;
}


nlohmann::json IdNode::to_json() const {
    nlohmann::json j;
    j["type"] = "id";
    j["id"]["_id"] = this->_id;
    return j;
}

