//
// Created by chris on 30/8/20.
//

#include "None.h"

using namespace ast;

bool None::equal(const ast::Node& other) const {
    return true;
}

None::None(TextPosition start, TextPosition end) : ast::Node(NodeType::NONE, start, end) {
}

nlohmann::json None::to_json() const {
    return {{"type", "none"}};
}

