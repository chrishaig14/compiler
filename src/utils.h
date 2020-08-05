//
// Created by chris on 2/8/20.
//

#ifndef UNTITLED1_UTILS_H
#define UNTITLED1_UTILS_H

#include "instructions/Instruction.h"
#include "nodes/nodes.h"

typedef std::vector<Node*> VectorOfNodes;
typedef std::vector<TypeNode*> VectorOfTypes;
typedef std::vector<std::string> VectorOfStrings;
typedef std::vector<Instruction*> Code;

#define ID(id) new IdNode(id)
#define NUM(num) new NumberNode(num)
#define BIN(op, left, right) new BinopNode(op, left, right)
#define MEM(parent, child) new MemberNode(parent, child)
#define IF(condition, then) new IfNode(condition, then)
#define CALL(function, arguments) new CallNode(function, arguments)
#define CLS(name, template_parameters, fields, methods) new ClassNode(name, template_parameters, fields, methods)
#define ASN(lvalue, rvalue) new AssignmentNode(lvalue, rvalue)
#define SUB(parent, child) new SubscriptNode(parent, child)
#define DECL(id, type, exp) new DeclarationNode(id, type, exp)
#define TYPE(parent, type_parameters) new TypeNode(parent, type_parameters)
#define T_BOOL new TypeNode("Boolean", {})
#define T_INT new TypeNode("Integer", {})
#define T_STRING new TypeNode("String", {})
#define T_LIST(e) TYPE("List", std::vector<TypeNode*>({e}))
#define T_DICT(k, v) TYPE("Dict", std::vector<TypeNode*>({k,v}))
#define FUN(a, b, c, d, e) new FunctionNode(a,b,c,d,e)

#define I_CALL new CallInst()
#define I_PUSHI(x) new PushIntegerInst(x)
#define I_PUSHS(x) new PushStringInst(x)
#define I_PUSHF(parameter_names, body, free_vars) new PushFunctionInst(parameter_names, body, free_vars)
#define I_SETS new SetSubscriptInst()
#define I_GETS new GetSubscriptInst()
#define I_SETM(x) new SetMemberInst(x)
#define I_GETM(x) new GetMemberInst(x)
#define I_SET(x) new SetInst(x)
#define I_GET(x) new GetInst(x)
#define I_DECL(x) new DeclareInst(x)
#define I_MAKE_LIST(length) new MakeListInst(length)
#define I_MAKE_OBJECT(a,b) new MakeObjectInst(a,b)
#define I_BIN(op) new BinopInst(op)
#define I_RET new ReturnInst()

#endif //UNTITLED1_UTILS_H
