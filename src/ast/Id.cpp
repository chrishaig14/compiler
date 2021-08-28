//
// Created by chris on 1/8/20.
//

#include "Id.h"
#include "../json/json.hpp"

using namespace ast;

Id::Id(std::string identifier, TextPosition start, TextPosition end) : ast::Node(NodeType::ID, start, end),
                                                                               _id(identifier) {
    this->is_global_function = false;
}


bool Id::equal(const ast::Node& x) const {
    const auto& other = (Id&) x;
    return this->_id == other._id;
}


nlohmann::json Id::to_json() const {
    nlohmann::json j;
    j["type"] = "id";
    j["id"]["_id"] = this->_id;
    return j;
}

