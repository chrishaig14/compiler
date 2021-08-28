//
// Created by chris on 19/6/21.
//

#include "ThrowSNode.h"

ThrowSNode::ThrowSNode(SNode* expression) : SNode(SNodeType::THROW), expression(expression) {
}
