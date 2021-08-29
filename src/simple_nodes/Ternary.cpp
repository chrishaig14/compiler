//
// Created by chris on 8/5/21.
//

#include "Ternary.h"

Ternary::Ternary(SNode* ext, SNode* true_case, SNode* false_case) : SNode(SNodeType::TERNARY) {
    this->ext = ext;
    this->true_case = true_case;
    this->false_case = false_case;
}
