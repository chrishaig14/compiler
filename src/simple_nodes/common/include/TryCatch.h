//
// Created by chris on 19/6/21.
//

#ifndef XLANG_TRYCATCH_H
#define XLANG_TRYCATCH_H

#include "Common.h"
#include "Block.h"

class sem::TryCatch : public sem::Common {
public:
    TryCatch(Block* body, std::vector<std::pair<std::string, std::string>> e_names_types,
                  std::vector<Common*> catches_bodies);

    std::vector<std::pair<std::string, std::string>> e_names_types;
    Block* body;
    std::vector<Common*> catches_bodies;
};


#endif //XLANG_TRYCATCH_H
