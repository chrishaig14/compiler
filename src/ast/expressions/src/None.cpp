//
// Created by chris on 30/8/20.
//

#include "../include/None.h"

using namespace ast;

bool None::equal(const ast::ExpNode& other) const {
    return true;
}

None::None(TextPosition start, TextPosition end) : ast::ExpNode(ExpNodeType::NONE, start, end) {
}

nlohmann::json None::to_json() const {
    return {{"type", "none"}};
}

