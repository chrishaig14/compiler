//
// Created by chris on 4/4/21.
//

#include "Return.h"

Return::Return(USNode& expression) : SNode(SNodeType::RETURN), expression(std::move(expression)) {
}
