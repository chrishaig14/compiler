//
// Created by chris on 8/8/20.
//

#ifndef UNTITLED1_DEFINESTRUCTINST_H
#define UNTITLED1_DEFINESTRUCTINST_H


#include "Instruction.h"
#include "../utils.h"

class DefineStructInst : public Instruction {
public:
    std::string identifier;
    std::map<std::string, Code> fields;

    DefineStructInst(const std::string& identifier, const std::map<std::string, Code>& fields);

    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor) override;

};


#endif //UNTITLED1_DEFINESTRUCTINST_H
