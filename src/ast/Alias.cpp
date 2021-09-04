//
// Created by chris on 28/4/21.
//

#include "Alias.h"

using namespace ast;

bool Alias::equal(const ast::Node& other) const {
    return false;
}

Alias::Alias(std::string alias_id, ast::TypeNode* aliased_type, TextPosition start, TextPosition end) : Node(NodeType::ALIAS,
                                                                                                        start,
                                                                                                        end) {
    this->alias_id = alias_id;
    this->aliased_type = aliased_type;
}

nlohmann::json Alias::to_json() const {
    return nlohmann::json();
}
