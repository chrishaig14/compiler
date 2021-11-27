//
// Created by chris on 11/4/21.
//

#ifndef XLANG_TEMPLATEKLASSDEF_H
#define XLANG_TEMPLATEKLASSDEF_H


#include <string>
#include "Top.h"
#include "FunctionDef.h"

class sem::TemplateKlassDef : public sem::Top {
public:
    std::string identifier;
    VectorOfStrings attributes;
    VectorOfStrings attribute_types;
    std::vector<std::unique_ptr<sem::FunctionDef>> methods;
    std::vector<std::unique_ptr<sem::FunctionDef>> static_methods;

    TemplateKlassDef(std::string identifier, VectorOfStrings attributes);

    bool equals(const Top& o) const override;
};


#endif //XLANG_TEMPLATEKLASSDEF_H
