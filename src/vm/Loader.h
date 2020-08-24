//
// Created by chris on 23/8/20.
//

#ifndef UNTITLED1_LOADER_H
#define UNTITLED1_LOADER_H


#include <vector>
#include "Environment.h"
#include "../utils.h"

class Loader {
public:
    Loader(CodeLabel allCode);

    void load();

    CodeLabel all_code;
    Environment* global_env;

    void load_function(std::string name, std::vector<std::pair<std::string, Instruction*>> code);

};


#endif //UNTITLED1_LOADER_H
