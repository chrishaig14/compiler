//
// Created by chris on 7/11/20.
//

#include "Continue.h"

using namespace ast;
using namespace ast;

bool Continue::equal(const ast::Node& other) const {
    return true;
}

Continue::Continue(TextPosition start, TextPosition end) : ast::Node(NodeType::CNTINUE, start, end) {
}

nlohmann::json Continue::to_json() const {
    return {{"type", "continue"}};
}

