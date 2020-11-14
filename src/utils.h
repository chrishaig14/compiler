//
// Created by chris on 2/8/20.
//

#ifndef UTILS_H
#define UTILS_H

#include "instructions/Instruction.h"
#include "nodes/nodes.h"

typedef std::vector<NodeContainer> VectorOfNodes;
typedef std::vector<TypeNode> VectorOfTypes;
typedef std::vector<std::string> VectorOfStrings;
typedef std::vector<Instruction*> Code;

#define ID(id) new IdNode(id)
#define NUM(num) new NumberNode(num)
#define BIN(op, left, right) new BinopNode(op, left, right)
#define MEM(parent, child) new MemberNode(parent, child)
#define IF(condition, then, elifs, selse) new IfNode(condition, then, elifs, selse)
#define IFELSE(condition, then, _else) new IfNode(condition, then, _else)
#define LST(list) new ListNode(list)
#define STR(str) new StringNode(str)
#define TERNARY(a, b, c) new TernaryNode(a,b,c)
#define CALL(function, arguments) new CallNode(function, arguments)
#define CLS(name, template_parameters, fields) new StructNode(name, template_parameters, fields)
#define ASN(lvalue, rvalue) new AssignmentNode(lvalue, rvalue)
#define FOR(var, exp, block) new ForNode(var, exp, block)
#define SUB(parent, child) new SubscriptNode(parent, child)
#define DECL(id, type, exp) new DeclarationNode(id, type, exp)
#define LIT_EXP(id, type) new ClassLiteralExpressionNode(id, type)
#define LIT_FIL(id, type) new ClassLiteralFieldNode(id, type)
typedef std::map<std::string, std::map<std::string, Code>> StructProtos;
#define TYPE(parent, type_parameters) NodeFactory::otype(parent, type_parameters)
#define BOOL(x) new BooleanNode(x)
#define T_BOOL  NodeFactory::otype("Boolean", {})
#define T_INT  NodeFactory::otype("Integer", {})
#define T_STRING  NodeFactory::otype("String", {})
#define T_LIST(e) NodeFactory::otype("List", {e})
#define T_DICT(k, v) TYPE("Dict", std::vector<TypeNode*>({k,v}))
#define FUN(a, b, c, d, e) new FunctionNode(a,b,c,d,e)
#define WHILE(condition, body) new WhileNode(condition, body)
#define OBJECT_TYPE(identifier, parameters) NodeFactory::otype(identifier, parameters)
#define FUNCTION_TYPE(parameters, return_type) NodeFactory::ftype(parameters, return_type)
#define RET(x) new ReturnNode(x)
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
typedef std::pair<std::string, TypeNode*> FieldInfo;
typedef std::vector<FieldInfo> StructFields;
typedef std::vector<std::pair<std::string, Instruction*>> CodeLabel;
#define TO_FUNCTION_TYPE(x) dynamic_cast<FunctionTypeNode*> (x)
#define TO_OBJECT_TYPE(x) dynamic_cast<ObjectTypeNode*> (x)
#define TO_ID(x) dynamic_cast<IdNode*> (x)
#define TO_SUB(x) dynamic_cast<SubscriptNode*> (x)
#define TO_MEMBER(x) dynamic_cast<MemberNode*>(x)
#define TO_CALL(x) dynamic_cast<CallNode*>(x)
#define TO_BINOP(x) dynamic_cast<BinopNode*>(x)
#define TO_NONE(x) dynamic_cast<NoneNode*>(x)

std::ostream& operator<<(std::ostream& out, const CodeLabel v);

bool operator==(const CodeLabel a, const CodeLabel b);

template<typename T>
bool item_in_vec(T item, std::vector<T> vec) {
    return std::find(vec.begin(), vec.end(), item) != vec.end();
}


#endif //UTILS_H
