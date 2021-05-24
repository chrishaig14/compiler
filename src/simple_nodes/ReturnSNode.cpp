//
// Created by chris on 4/4/21.
//

#include "ReturnSNode.h"

ReturnSNode::ReturnSNode(SNode* expression) : SNode(SNodeType::RETURN), expression(expression) {
}
