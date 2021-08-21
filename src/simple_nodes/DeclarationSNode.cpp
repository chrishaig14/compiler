//
// Created by chris on 4/4/21.
//

#include "DeclarationSNode.h"

DeclarationSNode::DeclarationSNode(std::string identifier, USNode& expression)
        : SNode(SNodeType::DECLARATION), identifier(identifier), expression(std::move(expression)) {
}
