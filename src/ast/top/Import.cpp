//
// Created by chris on 24/12/20.
//

#include "Import.h"

using namespace ast;

bool Import::equal(const ast::TopNode& other) const {
    return false;
}

nlohmann::json Import::to_json() const {
    // return {{"type",   "import"},
    //         {"import", this->path}};
    return "";
}

Import::Import(std::vector<Token> path, std::string alias, TextPosition start, TextPosition end) : ast::TopNode(
        TopNodeType::IMPORT,
        start,
        end) {
    this->alias = std::move(alias);
    this->path = path;
    this->has_alias = true;
}

Import::Import(std::vector<Token> path) : ast::TopNode(TopNodeType::IMPORT, path.front().start, path.back().end_pos) {
    this->has_alias = false;
    this->path = path;
}
