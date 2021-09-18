//
// Created by chris on 18/9/21.
//

#include "ObjectConstructor.h"

using namespace sem;

ObjectConstructorCall::ObjectConstructorCall(Path class_path, std::vector<USNode> arguments)
        : SNode(SNodeType::OBJECT_CONSTRUCTOR), class_path(class_path), arguments(std::move(arguments)) {
}

bool ObjectConstructorCall::equals(const SNode& o) const {
    return this->class_path.as_str() == ((const ObjectConstructorCall&) o).class_path.as_str();
}
