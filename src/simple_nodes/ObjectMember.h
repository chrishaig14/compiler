//
// Created by chris on 11/4/21.
//

#ifndef XLANG_OBJECTMEMBER_H
#define XLANG_OBJECTMEMBER_H


#include <string>
#include "SNode.h"
#include "../ast/TypeNode.h"
#include "../ast/Path.h"

class sem::ObjectMember : public sem::SNode {
public:
    SNode* object;
    Path class_path;
    std::string member_name;

    ObjectMember(SNode* object, Path class_path, std::string member_name)
            : SNode(SNodeType::OBJECT_MEMBER), object(object), class_path(class_path), member_name(member_name) {
    }

    bool equals(const SNode& o) const override {
        auto& other = (const ObjectMember&) o;
        return *this->object == *other.object && this->class_path.as_str() == other.class_path.as_str() &&
               this->member_name == other.member_name;
    }

};


#endif //XLANG_OBJECTMEMBER_H
