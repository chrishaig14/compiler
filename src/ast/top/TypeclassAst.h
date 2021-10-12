#ifndef XLANG_TYPECLASSAST_H
#define XLANG_TYPECLASSAST_H


#include "../statements/Statement.h"
#include "../general/TypeFunction.h"
#include "../top/TopNode.h"

class ast::TypeclassAst : public ast::TopNode {
public:
    std::string id;
    std::string base_type;
    std::unordered_map<std::string, ast::UFunctionType> methods;
    TypeclassAst(const std::string& id, const std::string& base_type,
              std::unordered_map<std::string, ast::UFunctionType> methods, TextPosition start, TextPosition end);
    nlohmann::json to_json() const override;
};

#endif //XLANG_TYPECLASSAST_H
