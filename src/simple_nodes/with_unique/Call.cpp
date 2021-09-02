//
// Created by chris on 4/4/21.
//

#include "Call.h"

using namespace sem;

Call::Call(USNode function, std::vector<USNode> arguments)
        : SNode(SNodeType::CALL), function(std::move(function)), arguments(std::move(arguments)) {
}
