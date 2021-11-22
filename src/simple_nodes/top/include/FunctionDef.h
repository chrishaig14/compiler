//
// Created by chris on 4/4/21.
//

#ifndef XLANG_FUNCTIONDEF_H
#define XLANG_FUNCTIONDEF_H

#include <string>
#include "Top.h"
#include <util/types.h>
#include <common/Path.h>
#include "../../common/include/Block.h"
#include <map>
#include <set>

class sem::FunctionDef : public sem::Top {
public:
    std::string identifier;
    VectorOfStrings params;
    std::unique_ptr<Block> body;
    std::map<std::string, std::set<std::string>> instances;
    FunctionDef(std::string identifier, VectorOfStrings params, std::unique_ptr<Block> body, std::map<std::string, std::set<std::string>> instances);
    bool equals(const Top& o) const override;
};


#endif //XLANG_FUNCTIONDEF_H
