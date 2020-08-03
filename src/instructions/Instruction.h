//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_INSTRUCTION_H
#define UNTITLED1_INSTRUCTION_H

#include <string>

class Instruction {
public:
    virtual bool equal(const Instruction* inst) const = 0;

    virtual std::string to_string() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Instruction& inst) {
        return os << inst.to_string();
    };

    virtual void accept(InstructionVisitor& visitor) = 0;

    void visit(InstructionVisitor& visitor) {
        visitor.visit(*this);
    }
};


#endif //UNTITLED1_INSTRUCTION_H