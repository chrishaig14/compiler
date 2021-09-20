//
// Created by chris on 6/9/20.
//

#include "EmptyList.h"

using namespace ast;

EmptyList::EmptyList(ast::UTypeNode type, TextPosition start, TextPosition end) : ast::ExpNode(ExpNodeType::EMPTYLST, start, end),
                                                                             type(std::move(type)) {
}

bool EmptyList::equal(const ast::ExpNode& other) const {
    return *((EmptyList&) other).type == *this->type;
}

EmptyList::~EmptyList() {
    // delete this->type;
}

nlohmann::json EmptyList::to_json() const {
    return {{"type",       "empty_list"},
            {"empty_list", {{"type", this->type->to_json()}}}};
}
