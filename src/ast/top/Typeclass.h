#ifndef XLANG_TYPECLASS_H
#define XLANG_TYPECLASS_H


#include "../common/CommonNode.h"
#include "../general/FunctionType.h"
#include "../ast.h"

class ast::Typeclass : public ast::CommonNode {
public:
    std::string id;
    std::string base_type;
    std::unordered_map<std::string, ast::UFunctionType> methods;
    Typeclass(const std::string& id, const std::string& base_type,
              std::unordered_map<std::string, ast::UFunctionType> methods, TextPosition start, TextPosition end);
    nlohmann::json to_json() const override;
    bool equal(const ast::CommonNode& other) const override;
};


#endif //XLANG_TYPECLASS_H
