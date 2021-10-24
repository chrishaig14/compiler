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
#include <util/Path.h>
#include <simple_nodes/sem.h>
#include <simple_nodes/common/include/TypeFunction.h>

class TypeclassFoo {
public:
    const std::string& id;
    const std::string& gen_type;
    std::map<std::string, sem::UTypeFunction> methods;
    Path path;
    TypeclassFoo(const std::string& id, const std::string& gen_type, Path path);
};


#endif //XLANG_TYPECLASSFOO_H
