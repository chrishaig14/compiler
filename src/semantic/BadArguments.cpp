//
// Created by chris on 2/8/20.
//

#include "BadArguments.h"

bool BadArguments::operator==(const BadArguments& other) const {
    return true;
}

BadArguments::BadArguments() : std::runtime_error("Bad Arguments") {
}
