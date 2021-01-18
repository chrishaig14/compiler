//
// Created by chris on 6/8/20.
//

#ifndef MAKECLASSINST_H
#define MAKECLASSINST_H


#include <vector>
#include "Instruction.h"
#include "../types.h"

class MakeClassInst : public Instruction {
public:
    std::string identifier;
    VectorOfStrings fields;

    MakeClassInst(const std::string& identifier, const VectorOfStrings& fields);

    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor) override;


};


#endif //MAKECLASSINST_H
