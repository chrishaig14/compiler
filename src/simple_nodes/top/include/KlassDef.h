//
// Created by chris on 11/4/21.
//

#ifndef XLANG_KLASSDEF_H
#define XLANG_KLASSDEF_H


#include <string>
#include "Top.h"
#include "FunctionDef.h"

class sem::KlassDef : public sem::Top {
public:
    std::string identifier;
    VectorOfStrings attributes;
    VectorOfStrings attribute_types;
    std::vector<sem::FunctionDef> methods;
    std::vector<sem::FunctionDef> static_methods;

    KlassDef(std::string identifier, VectorOfStrings attributes);

    bool equals(const Top& o) const override;
};


#endif //XLANG_KLASSDEF_H
