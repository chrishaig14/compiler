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
    VectorOfStrings members;
    std::vector<std::unique_ptr<sem::FunctionDef>> methods;
    std::vector<std::unique_ptr<sem::FunctionDef>> static_methods;

    KlassDef(std::string identifier, VectorOfStrings members);

    bool equals(const Top& o) const override;
};


#endif //XLANG_KLASSDEF_H
