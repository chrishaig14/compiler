//
// Created by chris on 1/5/21.
//

#include "EnumMemberSNode.h"

EnumMemberSNode::EnumMemberSNode(const std::string& enum_name, const std::string& value) : SNode(SNodeType::ENUM_MEMBER) {
    this->enum_name = enum_name;
    this->value = value;
}
