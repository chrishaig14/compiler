//
// Created by chris on 4/4/21.
//

#include "Assignment.h"

using namespace sem;
Assignment::Assignment(USNode& lvalue, USNode& rvalue)
        : SNode(SNodeType::ASSIGNMENT), lvalue(std::move(lvalue)), rvalue(std::move(rvalue)) {
}
