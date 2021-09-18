//
// Created by chris on 19/6/21.
//

#include "../include/Throw.h"
using namespace sem;
Throw::Throw(SNode* expression) : SNode(SNodeType::THROW), expression(expression) {
}
