//
// Created by chris on 17/11/20.
//

#ifndef UNTITLED1_MACROS_H
#define UNTITLED1_MACROS_H

#include "../units/entities/Entity.h"

#define ID(id) new ast::IdNode(id)
#define NUM(num) new NumberNode(num)
#define BIN(op, left, right) new BinopNode(op, left, right)
#define MEM(parent, child) new ast::MemberNode(parent, child)
#define IF(condition, then, elifs, selse) new ast::IfNode(condition, then, elifs, selse)
#define IFELSE(condition, then, _else) new ast::IfNode(condition, then, _else)
#define LST(list) new ListNode(list)
#define STR(str) new ast::StringNode(str)
#define TERNARY(a, b, c) new TernaryNode(a,b,c)
#define CALL(function, arguments) new ast::CallNode(function, arguments)
#define CLS(name, template_parameters, fields) new StructNode(name, template_parameters, fields)
#define ASN(lvalue, rvalue) new ast::AssignmentNode(lvalue, rvalue)
#define FOR(var, exp, block) new ast::ForNode(var, exp, block)
#define SUB(parent, child) new SubscriptNode(parent, child)
#define DECL(id, type, exp) new ast::DeclarationNode(id, type, exp)
#define TYPE(parent, type_parameters) new ast::ObjectType(parent, type_parameters)
#define BOOL(x) new ast::BooleanNode(x)

#define T_DICT(k, v) TYPE("Dict", ast::VectorOfTypes({k,v}))
#define FUN(a, b, c, d, e) new ast::FunctionNode(a,b,c,d,e)
#define WHILE(condition, body) new ast::WhileNode(condition, body)
#define FUNCTION_TYPE(parameters, return_type) new ast::FunctionType(parameters, return_type)
#define RET(x) new ast::ReturnNode(x)
#define I_ENTER(x) new EnterScope(x)
#define I_LEAVE(x) new LeaveScope(x)
#define I_CALL new CallInst()
#define I_PUSHB(x) new PushBooleanInst(x)
#define I_PUSHI(x) new PushIntegerInst(x)
#define I_PUSHS(x) new PushStringInst(x)
#define I_PUSHF(parameter_names, body) new PushFunctionInst(parameter_names, body)
#define I_SETS new SetSubscriptInst()
#define I_GETS new GetSubscriptInst()
#define I_SETM(x) new SetMemberInst(x)
#define I_JUMPN(x) new JumpIfNone(x)
#define I_PUSHN new PushNone()
#define I_MAKE_CLASS(identifier, fields) new MakeClassInst(identifier, fields)
#define I_GETM(x) new GetMemberInst(x)
#define I_SET(x) new SetInst(x)
#define I_GET(x) new GetInst(x)
#define I_DECL(x) new DeclareInst(x)
#define I_END_FUNCTION(name) new EndFunction(name)
#define I_START_FUNCTION(name) new StartFunction(name)
#define I_MAKE_LIST(length) new MakeListInst(length)
#define I_MAKE_OBJECT(identifier, fields) new MakeObjectInst(identifier, fields)
#define I_BIN(op) new BinopInst(op)
#define I_RET new ReturnInst()
#define I_POP new PopInst()
#define I_JUMPF(offset) new JumpIfFalseInst(offset)
#define I_JUMP(offset) new JumpInst(offset)
#define LC(label, instruction) std::pair<std::string, Instruction*>(label, instruction)
#define NBREAK new BreakNode()
#define NCONTINUE new ContinueNode()
#define NL(instruction) std::pair<std::string, Instruction*>("", instruction)
#define TO_FUNCTION_TYPE(x) dynamic_cast<Functionast::TypeNode*> (x)
#define TO_OBJECT_TYPE(x) dynamic_cast<Objectast::TypeNode*> (x)
#define TO_ID(x) dynamic_cast<ast::IdNode*> (x)
#define TO_SUB(x) dynamic_cast<SubscriptNode*> (x)
#define TO_MEMBER(x) dynamic_cast<ast::MemberNode*>(x)
#define TO_CALL(x) dynamic_cast<ast::CallNode*>(x)
#define TO_BINOP(x) dynamic_cast<BinopNode*>(x)
#define TO_NONE(x) dynamic_cast<NoneNode*>(x)

// Builtin primitive types

#define T_BOOL  ast::ObjectType("Boolean")
#define T_INT  ast::ObjectType("Integer")
#define T_STRING  ast::ObjectType("String")
#define T_LIST(e) ast::ObjectType("List", {e})
#define T_FLOAT ast::ObjectType("Float")
#define T_DOUBLE ast::ObjectType("Double")

typedef std::unordered_map<std::string, std::unique_ptr<Entity>> MapStringEntity;
typedef std::unordered_map<std::string, ast::Type*> MapStringType;

#endif //UNTITLED1_MACROS_H
