//
// Created by chris on 23/8/20.
//

#ifndef ENDFUNCTION_H
#define ENDFUNCTION_H


#include "Instruction.h"

class EndFunction : public Instruction {
public:
    std::string name;

    EndFunction(const std::string& name);

    bool equal(const Instruction* inst) const override;

    std::string to_string() const override;

    void accept(InstructionVisitor& visitor) override;

};


#endif //ENDFUNCTION_H
