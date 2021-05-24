//
// Created by chris on 11/4/21.
//

#ifndef XLANG_OBJECTMEMBERSNODE_H
#define XLANG_OBJECTMEMBERSNODE_H


#include <string>
#include "SNode.h"
#include "../nodes/TypeNode.h"

class ObjectMemberSNode : public SNode {
public:
    SNode* object;
    Path class_path;
    std::string member_name;

    ObjectMemberSNode() : SNode(SNodeType::OBJECT_MEMBER) {
        this->object = nullptr;
    }
};


#endif //XLANG_OBJECTMEMBERSNODE_H
