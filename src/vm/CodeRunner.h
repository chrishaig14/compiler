//
// Created by chris on 20/7/20.
//

#ifndef CODERUNNER_H
#define CODERUNNER_H


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
#include "../instructions/GetTupleMemberInst.h"

class CodeRunner : public InstructionVisitor {
public:

    const Code& code;
    size_t inst_ptr;
    ObjectStack& stack;
    Environment* env;
    std::unordered_map<std::string, std::vector<std::string>> classes;

    CodeRunner(const Code& code, std::unordered_map<std::string, std::unordered_map<std::string, Code>>& structs,
               ObjectStack& stack,
               Environment* global_env);

    void run();

    void visit(BinopInst& inst) override;

    void visit(CallInst& call) override;

    void visit(DeclareInst& declare) override;

    void visit(GetInst& inst) override;

    void visit(GetMemberInst& inst) override;

    void visit(GetTupleMemberInst& inst) override;

    void visit(GetSubscriptInst& inst) override;

    void visit(PushFunctionInst& inst) override;

    void visit(PushIntegerInst& inst) override;

    void visit(MakeTupleInst& inst) override;

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

    void visit(JumpInst& inst) override;

    void visit(LeaveScope& inst) override;

    void visit(EnterScope& inst) override;

    void visit(NopInst& inst) override;

    void visit(EndFunction& inst) override;

    void visit(StartFunction& inst) override;

    void visit(JumpIfNone& inst) override;

    void visit(PushNone& inst) override;

    void visit(PopInst& inst) override;

    std::unordered_map<std::string, std::unordered_map<std::string, Code>>& structs;
    Environment* global_env;
};


#endif //CODERUNNER_H
