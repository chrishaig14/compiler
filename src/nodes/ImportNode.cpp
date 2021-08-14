//
// Created by chris on 24/12/20.
//

#include "ImportNode.h"

bool ImportNode::equal(const Node& other) const {
    return false;
}

nlohmann::json ImportNode::to_json() const {
    return {{"type",   "import"},
            {"import", this->path}};
}
