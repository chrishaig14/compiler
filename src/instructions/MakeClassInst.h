//
// Created by chris on 6/8/20.
//

#ifndef MAKECLASSINST_H
#define MAKECLASSINST_H


#include <vector>
#include "Instruction.h"

class MakeClassInst : public Instruction {
public:
    std::string identifier;
    std::vector<std::string> fields;

    MakeClassInst(const std::string& identifier, const std::vector<std::string>& fields);

    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor) override;


};


#endif //MAKECLASSINST_H
