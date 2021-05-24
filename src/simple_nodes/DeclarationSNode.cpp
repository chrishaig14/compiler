//
// Created by chris on 4/4/21.
//

#include "DeclarationSNode.h"

DeclarationSNode::DeclarationSNode(std::string identifier, SNode* expression)
        : SNode(SNodeType::DECLARATION), identifier(identifier), expression(expression) {
}
