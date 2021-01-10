//
// Created by chris on 23/8/20.
//

#ifndef LOADER_H
#define LOADER_H


#include <vector>
#include "Environment.h"
#include "../utils.h"

class Loader {
public:
    CodeObject* main;
    Loader(CodeLabel allCode, std::vector<std::pair<std::string, CodeBuiltin>>& builtins);

    void load();

    CodeLabel all_code;
    Environment* global_env;

    void load_function(std::string name, std::vector<std::pair<std::string, Instruction*>> code);

};


#endif //LOADER_H
