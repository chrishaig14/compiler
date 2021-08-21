//
// Created by chris on 4/4/21.
//

#include "ReturnSNode.h"

ReturnSNode::ReturnSNode(USNode& expression) : SNode(SNodeType::RETURN), expression(std::move(expression)) {
}
