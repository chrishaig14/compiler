//
// Created by chris on 11/4/21.
//

#ifndef XLANG_OBJECTMEMBER_H
#define XLANG_OBJECTMEMBER_H


#include <string>
#include "../SNode.h"
#include "../../ast/Type.h"
#include "../../ast/Path.h"

class sem::ObjectMember : public sem::SNode {
public:
    USNode object;
    Path class_path;
    std::string member_name;

    ObjectMember(USNode object, Path class_path, std::string member_name)
            : SNode(SNodeType::OBJECT_MEMBER), object(std::move(object)), class_path(class_path), member_name(member_name) {
    }

    bool equals(const SNode& o) const override {
        auto& other = (const ObjectMember&) o;
        bool obj_ok = *this->object == *other.object;
        bool class_ok = this->class_path.as_str() == other.class_path.as_str();
        bool member_ok = this->member_name == other.member_name;
        return obj_ok && class_ok && member_ok;
    }

};


#endif //XLANG_OBJECTMEMBER_H
