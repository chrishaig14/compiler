//
// Created by chris on 28/4/21.
//

#include "AliasNode.h"

bool AliasNode::equal(const Node& other) const {
    return false;
}

AliasNode::AliasNode(std::string alias_id, TypeNode* aliased_type, TextPosition start, TextPosition end) :Node(NodeType::ALIAS, start, end){
    this->alias_id = alias_id;
    this->aliased_type = aliased_type;
}

nlohmann::json AliasNode::to_json() const {
    return nlohmann::json();
}
