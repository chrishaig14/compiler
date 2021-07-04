//
// Created by chris on 11/4/21.
//

#ifndef XLANG_OBJECTMEMBERSNODE_H
#define XLANG_OBJECTMEMBERSNODE_H


#include <string>
#include "SNode.h"
#include "../nodes/TypeNode.h"
#include "../nodes/Path.h"

class ObjectMemberSNode : public SNode {
public:
    SNode* object;
    Path class_path;
    std::string member_name;

    ObjectMemberSNode(SNode* object, Path class_path, std::string member_name)
            : SNode(SNodeType::OBJECT_MEMBER), object(object), class_path(class_path), member_name(member_name) {
    }
};


#endif //XLANG_OBJECTMEMBERSNODE_H
