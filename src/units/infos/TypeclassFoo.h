//
// Created by chris on 11/10/21.
//

#ifndef XLANG_TYPECLASSFOO_H
#define XLANG_TYPECLASSFOO_H

#include <string>
#include <unordered_map>
#include <string>
#include <vector>
#include <map>
#include "../../util/Path.h"

class TypeclassFoo {
public:
    Path path;
    const std::string& id;
    const std::string& gen_type;
    TypeclassFoo(const std::string& id, const std::string& gen_type);
};


#endif //XLANG_TYPECLASSFOO_H
