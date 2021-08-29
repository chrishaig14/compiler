//
// Created by chris on 4/4/21.
//

#include "Return.h"
using namespace sem;
Return::Return(USNode& expression) : SNode(SNodeType::RETURN), expression(std::move(expression)) {
}
