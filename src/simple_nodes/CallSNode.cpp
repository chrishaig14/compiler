//
// Created by chris on 4/4/21.
//

#include "CallSNode.h"

CallSNode::CallSNode(SNode* function, std::vector<SNode*> arguments)
        : SNode(SNodeType::CALL), function(function), arguments(arguments) {
}
