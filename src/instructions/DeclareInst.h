//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_DECLAREINST_H
#define UNTITLED1_DECLAREINST_H


#include <string>
#include "Instruction.h"

class DeclareInst : public Instruction {
public:
    std::string identifier;

    DeclareInst(std::string identifier);

    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;
};


#endif //UNTITLED1_DECLAREINST_H
