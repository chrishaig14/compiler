//
// Created by chris on 29/8/21.
//

#ifndef XLANG_OBJECTMETHOD_H
#define XLANG_OBJECTMETHOD_H

#include <string>
#include "../../common/include/SNode.h"
#include "../../../ast/Type.h"
#include "../../../ast/Path.h"

class sem::ObjectMethod : public sem::SNode {
public:
    USNode object;
    Path class_path;
    std::string method_name;

    ObjectMethod(USNode object, Path class_path, std::string method_name)
            : SNode(SNodeType::OBJECT_METHOD), object(std::move(object)), class_path(class_path),
              method_name(method_name) {
    }

    bool equals(const SNode& o) const override {
        auto& other = (const ObjectMethod&) o;
        bool obj_ok = *this->object == *other.object;
        bool class_ok = this->class_path.as_str() == other.class_path.as_str();
        bool member_ok = this->method_name == other.method_name;
        return obj_ok && class_ok && member_ok;
    }

};


#endif //XLANG_OBJECTMETHOD_H
