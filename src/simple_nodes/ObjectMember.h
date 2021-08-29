//
// Created by chris on 11/4/21.
//

#ifndef XLANG_OBJECTMEMBER_H
#define XLANG_OBJECTMEMBER_H


#include <string>
#include "SNode.h"
#include "../ast/TypeNode.h"
#include "../ast/Path.h"

class ObjectMember : public SNode {
public:
    SNode* object;
    Path class_path;
    std::string member_name;

    ObjectMember(SNode* object, Path class_path, std::string member_name)
            : SNode(SNodeType::OBJECT_MEMBER), object(object), class_path(class_path), member_name(member_name) {
    }
};


#endif //XLANG_OBJECTMEMBER_H
