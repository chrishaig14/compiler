//
// Created by chris on 19/6/21.
//

#include "../include/TryCatch.h"
using namespace sem;

TryCatch::TryCatch(Block* body, std::vector<std::pair<std::string, std::string>> e_names_types,
                   std::vector<Common*> catches_bodies) : Common(CommonType::TRY_CATCH) {
    this->body = body;
    this->catches_bodies = catches_bodies;
    this->e_names_types = e_names_types;
}
