//
// Created by chris on 7/6/20.
//

#ifndef ASTNODE_H
#define ASTNODE_H

#include <iostream>
#include <vector>
#include <map>


enum class BinopType {
    AND, OR, PLUS, MINUS, TIMES, DIV, EQ
};

enum class AstType {
    RETURN, LIST, CLASS, IF, FUNCTION, MEMBER, ASSIGNMENT, BINOP, IDENTIFIER, DECLARATION, TYPE, NUMBER, CALL, SUB,
    STRING
};

std::string ast_string(AstType type);

class ReturnNode;

class FunctionNode;

class ClassNode;

class NumberNode;

class StringNode;

class IfNode;

class BinopNode;

class AssignmentNode;

class IdentifierNode;

class DeclarationNode;

class ListNode;

class MemberNode;

class TypeNode;

class CallNode;

class SubscriptNode;

class AstNode {
public:
    union {
        ReturnNode* ast_return;
        FunctionNode* ast_function;
        ClassNode* ast_class;
        IfNode* ast_if;
        BinopNode* ast_binop;
        StringNode* ast_string;
        AssignmentNode* ast_assignment;
        ListNode* ast_list;
        IdentifierNode* ast_identifier;
        MemberNode* ast_member;
        DeclarationNode* ast_declaration;
        TypeNode* ast_type;
        NumberNode* ast_number;
        CallNode* ast_call;
        SubscriptNode* ast_sub;
    };
    AstType type;
};

typedef std::vector<TypeNode*> VectorOfTypes;
typedef std::vector<std::string> VectorOfStrings;
typedef std::vector<AstNode*> VectorOfNodes;

bool equal(VectorOfStrings a, VectorOfStrings b);

bool equal(TypeNode* a, TypeNode* b);

bool compare(AssignmentNode* a, AssignmentNode* b);

bool equal(NumberNode* a, NumberNode* b);

bool equal(StringNode* a, StringNode* b);

bool compare(ReturnNode* a, ReturnNode* b);

bool equal(ClassNode* a, ClassNode* b);

bool equal(CallNode* a, CallNode* b);

bool equal(SubscriptNode* a, SubscriptNode* b);

bool equal(FunctionNode* a, FunctionNode* b);

bool equal(IfNode* a, IfNode* b);

bool compare(BinopNode* a, BinopNode* b);

bool compare(IdentifierNode* a, IdentifierNode* b);

bool equal(DeclarationNode* a, DeclarationNode* b);

bool compare(ListNode* a, ListNode* b);

bool equal(VectorOfNodes a, VectorOfNodes b);

bool compare(VectorOfTypes a, VectorOfTypes b);

bool compare(MemberNode* a, MemberNode* b);

bool equal(AstNode* a, AstNode* b);

class ReturnNode {
public:
    AstNode* expression;

    ReturnNode(AstNode* expression) {
        this->expression = expression;
    }
};

class ListNode {
public:
    VectorOfNodes elements;

    ListNode(VectorOfNodes elements) {
        this->elements = elements;
    }
};

class IfNode {
public:
    AstNode condition;
    VectorOfNodes then;

    IfNode(AstNode condition, VectorOfNodes then) {
        this->condition = condition;
        this->then = then;
    }
};

class NumberNode {
public:
    int num;

    NumberNode(int num) {
        this->num = num;
    }
};

class StringNode {
public:
    std::string str;

    StringNode(std::string str) {
        this->str = str;
    }
};

class CallNode {
public:
    AstNode function;
    VectorOfNodes arguments;

    CallNode(AstNode function, VectorOfNodes arguments) {
        this->function = function;
        this->arguments = arguments;
    }
};

//CallNode* mk_call(AstNode* function, )



class SubscriptNode {
public:
    AstNode parent;
    AstNode sub;

    SubscriptNode(AstNode parent, AstNode sub) {
        this->parent = parent;
        this->sub = sub;
    }
};


class BinopNode {
public:
    BinopType op;
    AstNode left;
    AstNode right;

    BinopNode(BinopType op, AstNode left, AstNode right) {
        this->op = op;
        this->left = left;
        this->right = right;
    }
};

class AssignmentNode {
public:
    AstNode lvalue;
    AstNode rvalue;

    AssignmentNode(AstNode lvalue, AstNode rvalue) {
        this->lvalue = lvalue;
        this->rvalue = rvalue;
    }
};

class TypeNode {
public:
    std::string name;
    VectorOfTypes type_parameters;

    TypeNode(std::string name, VectorOfTypes type_parameters) {
        this->name = name;
        this->type_parameters = type_parameters;
    }
};

class IdentifierNode {
public:
    std::string name;

    IdentifierNode(std::string name) {
        this->name = name;
    }
};

class DeclarationNode {
public:
    std::string identifier;
    TypeNode* type;
    AstNode* expression;

    DeclarationNode(std::string identifier,
                    TypeNode* type, AstNode* expression) {
        this->identifier = identifier;
        this->type = type;
        this->expression = expression;
    }
};

class FunctionNode {
public:
    VectorOfTypes parameter_types;
    VectorOfStrings parameter_names;
    std::string name;
    TypeNode* return_type;
    VectorOfNodes body;
    std::map<std::string, int> free_variables;

    FunctionNode(std::string name, VectorOfStrings parameter_names, VectorOfTypes parameter_types,
                 TypeNode* return_type, VectorOfNodes body) {
        this->name = name;
        this->parameter_types = parameter_types;
        this->parameter_names = parameter_names;
        this->return_type = return_type;
        this->body = body;
    }
};

class ClassNode {
public:
    std::string name;
    VectorOfStrings template_parameters;
    std::vector<DeclarationNode*> fields;
    std::vector<FunctionNode*> methods;

    ClassNode(std::string name,
              VectorOfStrings template_parameters,
              std::vector<DeclarationNode*> fields,
              std::vector<FunctionNode*> methods) {
        this->name = name;
        this->template_parameters = template_parameters;
        this->fields = fields;
        this->methods = methods;
    }
};


class FunctionSignatureNode {
public:
    VectorOfTypes parameter_types;
    std::string name;
    TypeNode* return_type;
    VectorOfNodes body;
};

class MemberNode {
public:
    AstNode parent;
    std::string child;

    MemberNode(AstNode parent, std::string child) {
        this->parent = parent;
        this->child = child;
    }
};


FunctionNode* i_fun(std::string name, VectorOfStrings parameter_names, VectorOfTypes parameter_types,
                    TypeNode* return_type, VectorOfNodes body);

DeclarationNode* i_decl(std::string name, AstNode* expression);

DeclarationNode* i_decl_type(std::string name, TypeNode* type, AstNode* expression);

ClassNode* i_class(std::string name,
                   VectorOfStrings template_parameters,
                   std::vector<DeclarationNode*> fields,
                   std::vector<FunctionNode*> methods);

IfNode* i_if(AstNode condition, VectorOfNodes then);

TypeNode* i_type(std::string name, VectorOfTypes type_parameters);

#endif //ASTNODE_H
