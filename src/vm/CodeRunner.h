//
// Created by chris on 20/7/20.
//

#ifndef UNTITLED1_CODERUNNER_H
#define UNTITLED1_CODERUNNER_H


#include <vector>
#include <stack>
#include <stdexcept>
#include "../instructions/instructions.h"
#include "Object.h"
#include "ObjectStack.h"
#include "Environment.h"
#include "IntegerObject.h"
#include "StringObject.h"
#include "ListObject.h"
#include "CodeObject.h"
#include "UserObject.h"
#include "BooleanObject.h"

class CodeRunner : public InstructionVisitor {
public:

    const Code& code;
    size_t inst_ptr;
    ObjectStack& stack;
    Environment* env;
    std::map<std::string, std::vector<std::string>> classes;

    CodeRunner(const Code& code, std::map<std::string, std::map<std::string, Code>>& structs, ObjectStack& stack,
               std::map<std::string, Object*> closure);

    void run();

    void visit(BinopInst& inst) override;

    void visit(CallInst& call) override;

    void visit(DeclareInst& declare) override;

    void visit(GetInst& inst) override;

    void visit(GetMemberInst& inst) override;

    void visit(GetSubscriptInst& inst) override;

    void visit(PushFunctionInst& inst) override;

    void visit(PushIntegerInst& inst) override;

    void visit(PushStringInst& inst) override;

    void visit(ReturnInst& inst) override;

    void visit(SetInst& inst) override;

    void visit(SetMemberInst& inst) override;

    void visit(SetSubscriptInst& inst) override;

    void visit(MakeObjectInst& inst) override;

    void visit(MakeListInst& inst) override;

    void visit(JumpIfFalseInst& inst) override;

    void visit(PushBooleanInst& inst) override;

    void visit(MakeClassInst& inst) override;

    void visit(DefineStructInst& inst) override;

    void visit(MakeDefaultInst& inst) override;

    std::map<std::string, std::map<std::string, Code>>& structs;
};


#endif //UNTITLED1_CODERUNNER_H
