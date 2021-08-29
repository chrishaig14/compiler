//
// Created by chris on 4/4/21.
//

#include "Declaration.h"

using namespace sem;

Declaration::Declaration(std::string identifier, USNode expression)
        : SNode(SNodeType::DECLARATION), identifier(identifier), expression(std::move(expression)) {
}
