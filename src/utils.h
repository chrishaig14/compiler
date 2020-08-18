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
#define LST(list) new ListNode(list)
#define CALL(function, arguments) new CallNode(function, arguments)
#define CLS(name, template_parameters, fields) new StructNode(name, template_parameters, fields)
#define ASN(lvalue, rvalue) new AssignmentNode(lvalue, rvalue)
#define FOR(var, exp, block) new ForNode(var, exp, block)
#define SUB(parent, child) new SubscriptNode(parent, child)
#define DECL(id, type, exp) new DeclarationNode(id, type, exp)
#define LIT_EXP(id, type) new ClassLiteralExpressionNode(id, type)
#define LIT_FIL(id, type) new ClassLiteralFieldNode(id, type)
typedef std::map<std::string, std::map<std::string, Code>> StructProtos;
#define TYPE(parent, type_parameters) new ObjectTypeNode(parent, type_parameters)
#define BOOL(x) new BooleanNode(x)
#define T_BOOL new ObjectTypeNode("Boolean", {})
#define T_INT new ObjectTypeNode("Integer", {})
#define T_STRING new ObjectTypeNode("String", {})
#define T_LIST(e) TYPE("List", std::vector<TypeNode*>({e}))
#define T_DICT(k, v) TYPE("Dict", std::vector<TypeNode*>({k,v}))
#define FUN(a, b, c, d, e) new FunctionNode(a,b,c,d,e)
#define WHILE(condition, body) new WhileNode(condition, body)
#define OBJECT_TYPE(identifier, parameters) new ObjectTypeNode(identifier, parameters)
#define FUNCTION_TYPE(parameters, return_type) new FunctionTypeNode(parameters, return_type)

#define I_CALL new CallInst()
#define I_PUSHB(x) new PushBooleanInst(x)
#define I_PUSHI(x) new PushIntegerInst(x)
#define I_PUSHS(x) new PushStringInst(x)
#define I_PUSHF(parameter_names, body, free_vars) new PushFunctionInst(parameter_names, body, free_vars)
#define I_SETS new SetSubscriptInst()
#define I_GETS new GetSubscriptInst()
#define I_SETM(x) new SetMemberInst(x)
#define I_MAKE_CLASS(identifier, fields) new MakeClassInst(identifier, fields)
#define I_GETM(x) new GetMemberInst(x)
#define I_SET(x) new SetInst(x)
#define I_GET(x) new GetInst(x)
#define I_DECL(x) new DeclareInst(x)
#define I_MAKE_LIST(length) new MakeListInst(length)
#define I_MAKE_OBJECT(identifier, fields) new MakeObjectInst(identifier, fields)
#define I_BIN(op) new BinopInst(op)
#define I_RET new ReturnInst()
#define I_JUMPF(offset) new JumpIfFalseInst(offset)
typedef std::map<std::string, TypeNode*> StructFields;
#endif //UNTITLED1_UTILS_H
