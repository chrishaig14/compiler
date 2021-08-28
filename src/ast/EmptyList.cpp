//
// Created by chris on 6/9/20.
//

#include "EmptyList.h"

using namespace ast;

EmptyList::EmptyList(UTypeNode type, TextPosition start, TextPosition end) : ast::Node(NodeType::EMPTYLST, start, end),
                                                                             type(std::move(type)) {
}

bool EmptyList::equal(const ast::Node& other) const {
    return *((EmptyList&) other).type == *this->type;
}

EmptyList::~EmptyList() {
    // delete this->type;
}

nlohmann::json EmptyList::to_json() const {
    return {{"type",       "empty_list"},
            {"empty_list", {{"type", this->type->to_json()}}}};
}
