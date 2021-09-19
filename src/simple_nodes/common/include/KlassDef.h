//
// Created by chris on 11/4/21.
//

#ifndef XLANG_KLASSDEF_H
#define XLANG_KLASSDEF_H


#include <string>
#include "Common.h"
#include "FunctionDef.h"

class sem::KlassDef : public sem::Common {
public:
    std::string identifier;
    VectorOfStrings members;
    std::vector<std::unique_ptr<sem::FunctionDef>> methods;

    KlassDef(std::string identifier, VectorOfStrings members);

    bool equals(const Common& o) const override;
};


#endif //XLANG_KLASSDEF_H
