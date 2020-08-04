//
// Created by chris on 2/8/20.
//

#include "PushFunctionInst.h"

PushFunctionInst::PushFunctionInst(const std::vector<std::string>& parameterNames, const Code& body,
                                   const std::map<std::string, int>& freeVariables) : parameter_names(parameterNames),
                                                                                      body(body),
                                                                                      free_variables(freeVariables) {}

void PushFunctionInst::accept(InstructionVisitor& visitor) {
    visitor.visit(*this);
}
