//
// Created by chris on 6/8/20.
//

#include "MakeClassInst.h"
#include "../types.h"

bool MakeClassInst::equal(const Instruction* inst) const {
    const MakeClassInst* other_ptr = dynamic_cast<const MakeClassInst*>(inst);
    return this->identifier == other_ptr->identifier && this->fields == other_ptr->fields;
    return false;
}

std::string MakeClassInst::to_string() const {
    std::string out = "MAKE_CLASS " + this->identifier + "(";
    for (auto f: this->fields) {
        out += f + ",";
    }
    out = out.substr(0, out.size() - 1);
    out += ")";
    return out;
}

void MakeClassInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}

MakeClassInst::MakeClassInst(const std::string& identifier, const VectorOfStrings& fields) : identifier(
        identifier), fields(fields) {}
