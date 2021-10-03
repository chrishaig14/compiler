#ifndef XLANG_TYPECLASS_H
#define XLANG_TYPECLASS_H


#include "../statements/Statement.h"
#include "../general/TypeFunction.h"
#include "../ast.h"

class ast::Typeclass : public ast::Statement {
public:
    std::string id;
    std::string base_type;
    std::unordered_map<std::string, ast::UFunctionType> methods;
    Typeclass(const std::string& id, const std::string& base_type,
              std::unordered_map<std::string, ast::UFunctionType> methods, TextPosition start, TextPosition end);
    nlohmann::json to_json() const override;
    bool equal(const ast::Statement& other) const override;
};


#endif //XLANG_TYPECLASS_H
