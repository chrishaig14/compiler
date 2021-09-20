//
// Created by chris on 18/8/20.
//

#include "Break.h"

using namespace ast;

bool Break::equal(const ast::CommonNode& other) const {
    return true;
}

Break::Break(TextPosition start, TextPosition end) : ast::CommonNode(CommonNodeType::BRK, start, end) {
}

nlohmann::json Break::to_json() const {
    return {{"type", "break"}};
}


