//
// Created by chris on 1/5/21.
//

#ifndef XLANG_ENUMMEMBERSNODE_H
#define XLANG_ENUMMEMBERSNODE_H


#include <string>
#include "SNode.h"

class EnumMemberSNode : public SNode {
public:
    EnumMemberSNode(std::string enum_name, std::string value) : SNode(SNodeType::ENUM_MEMBER) {
        this->enum_name = enum_name;
        this->value = value;
    }

    std::string enum_name;
    std::string value;
};


#endif //XLANG_ENUMMEMBERSNODE_H
