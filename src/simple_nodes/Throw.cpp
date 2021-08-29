//
// Created by chris on 19/6/21.
//

#include "Throw.h"
using namespace sem;
Throw::Throw(SNode* expression) : SNode(SNodeType::THROW), expression(expression) {
}
