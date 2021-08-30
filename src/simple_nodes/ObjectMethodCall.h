//
// Created by chris on 29/8/21.
//

#ifndef XLANG_OBJECTMETHODCALL_H
#define XLANG_OBJECTMETHODCALL_H

#include <string>
#include "SNode.h"
#include "../ast/TypeNode.h"
#include "../ast/Path.h"

class sem::ObjectMethodCall : public sem::SNode {
public:
    USNode object;
    Path class_path;
    std::vector<USNode> args;
    std::string method_name;

    ObjectMethodCall(USNode object, Path class_path, const std::string& method_name, std::vector<USNode> args) : SNode(
            SNodeType::OBJECT_METHOD_CALL), object(std::move(object)), class_path(class_path), args(std::move(args)),
                                                                                                                 method_name(
                                                                                                                         method_name) {
    }

    bool equals(const SNode& o) const override {
        auto& other = (const ObjectMethodCall&) o;
        bool obj_ok = *this->object == *other.object;
        bool class_ok = this->class_path.as_str() == other.class_path.as_str();
        bool method_ok = this->method_name == other.method_name;
        bool args_ok = true;
        if (this->args.size() != other.args.size()) {
            return false;
        }
        for (size_t i = 0; i < this->args.size(); i++) {
            if (*this->args[i] != *other.args[i]) {
                return false;
            }
        }
        return obj_ok && class_ok && args_ok && method_ok;
    }

};

#endif //XLANG_OBJECTMETHODCALL_H
