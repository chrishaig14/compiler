//
// Created by chris on 4/4/21.
//

#include "Call.h"

Call::Call(SNode* function, std::vector<SNode*> arguments)
        : SNode(SNodeType::CALL), function(function), arguments(arguments) {
}
