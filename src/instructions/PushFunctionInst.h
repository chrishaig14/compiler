//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_PUSHFUNCTIONINST_H
#define UNTITLED1_PUSHFUNCTIONINST_H


#include <vector>
#include <string>
#include "Instruction.h"
#include "../translator/Translator.h"

class PushFunctionInst : public Instruction {
public:
    PushFunctionInst(const std::vector<std::string>& parameterNames, const Code& body,
                     const std::map<std::string, int>& freeVariables);

public:

private:
    std::vector<std::string> parameter_names;
    Code body;
    std::map<std::string, int> free_variables;
};


#endif //UNTITLED1_PUSHFUNCTIONINST_H
