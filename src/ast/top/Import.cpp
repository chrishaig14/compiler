//
// Created by chris on 24/12/20.
//

#include "Import.h"

using namespace ast;

bool Import::equal(const ast::TopNode& other) const {
    return false;
}

nlohmann::json Import::to_json() const {
    return {{"type",   "import"},
            {"import", this->path}};
}

Import::Import(const VectorOfStrings& path, std::string alias, TextPosition start, TextPosition end) : ast::TopNode(TopNodeType::IMPORT, start, end) {
    this->alias = std::move(alias);
    this->path = path;
    this->has_alias = true;
}

Import::Import(const VectorOfStrings& path, TextPosition start, TextPosition end) : ast::TopNode(TopNodeType::IMPORT,
                                                                                              start,
                                                                                              end) {
    this->has_alias = false;
    this->path = path;
}
