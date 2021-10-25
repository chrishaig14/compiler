//
// Created by chris on 12/8/21.
//

#include "../include/ListLiteral.h"
#include <simple_nodes/common/include/TypeObject.h>

using namespace error;

Error* ListLiteral::clone() const {
    return new ListLiteral(this->node, this->type, this->position, this->node_1);
}

bool ListLiteral::equal(const Error& other) const {
    const ListLiteral& o = (const ListLiteral&) other;
    bool node_ok = this->node == o.node;
    bool type_ok = this->type == o.type;
    bool node_1_ok = this->node_1 == this->node_1;
    return node_ok and type_ok and node_1_ok;
}

std::string ListLiteral::to_str(const CodeLines& code) const {
    return std::string();
}

ListLiteral::ListLiteral(const sem::Type& node, const sem::TypeObject& type, TextPosition position,
                         const ast::Statement& node_1)
        : node(node), type(type), position(position), node_1(node_1) {

}
