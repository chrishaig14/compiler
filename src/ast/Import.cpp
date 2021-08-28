//
// Created by chris on 24/12/20.
//

#include "Import.h"
using namespace ast;
bool Import::equal(const ast::Node& other) const {
    return false;
}

nlohmann::json Import::to_json() const {
    return {{"type",   "import"},
            {"import", this->path}};
}
