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

class sem::FunctionDef : public sem::Top {
public:
    std::string identifier;
    VectorOfStrings params;
    std::unique_ptr<Block> body;
    std::vector<std::pair<Path,Path>> instances;
    FunctionDef(std::string identifier, VectorOfStrings params, std::unique_ptr<Block> body, std::vector<std::pair<Path, Path>> instances);
    bool equals(const Top& o) const override;
};


#endif //XLANG_FUNCTIONDEF_H
