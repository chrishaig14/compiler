//
// Created by chris on 7/6/20.
//

#include "AstNode.h"
#include "FirstPass.h"

FunctionNode*
i_fun(std::string name, VectorOfStrings parameter_names, VectorOfTypes parameter_types, TypeNode* return_type,
      VectorOfNodes body) {
    return new FunctionNode(name, parameter_names, parameter_types, return_type, body);
}

DeclarationNode* i_decl(std::string name, AstNode* expression) {
    return new DeclarationNode(name, nullptr, expression);
}

DeclarationNode* i_decl_type(std::string name, TypeNode* type, AstNode* expression) {
    return new DeclarationNode(name, type, expression);
}

ClassNode* i_class(std::string name, VectorOfStrings template_parameters,
                   std::vector<DeclarationNode*> fields, std::vector<FunctionNode*> methods) {
    return new ClassNode(name, template_parameters, fields, methods);
}

IfNode* i_if(AstNode condition, VectorOfNodes then) {
    return new IfNode(condition, then);
}

TypeNode* i_type(std::string name, VectorOfTypes type_parameters) {
    return new TypeNode(name, type_parameters);
}


bool equal(VectorOfStrings a, VectorOfStrings b) {
    if (a.size() != b.size()) return false;
    for (int i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

bool equal(VectorOfNodes a, VectorOfNodes b) {
    if (a.size() != b.size()) return false;
    for (int i = 0; i < a.size(); i++) {
        if (not equal(a[i], b[i])) return false;
    }
    return true;
}

bool compare(VectorOfTypes a, VectorOfTypes b) {
    if (a.size() != b.size()) return false;
    for (int i = 0; i < a.size(); i++) {
        if (not equal(a[i], b[i])) return false;
    }
    return true;
}

bool equal(NumberNode* a, NumberNode* b) {
    return a->num == b->num;
}

bool equal(StringNode* a, StringNode* b) {
    return a->str == b->str;
}

bool equal(TypeNode* a, TypeNode* b) {
    if (a == nullptr and b == nullptr) {
        return true;
    }
    if (a->name != b->name) return false;
    if (a->type_parameters.size() != b->type_parameters.size()) return false;
    for (int i = 0; i < a->type_parameters.size(); i++) {
        if (not equal(a->type_parameters[i], b->type_parameters[i])) return false;
    }
    return true;
}

bool compare(MemberNode* a, MemberNode* b) {
    if (a == nullptr and b == nullptr) {
        return true;
    }
    return equal(a->parent, b->parent) and (a->child == b->child);
}

std::string ast_string(AstType type) {
    switch (type) {
        case AstType::RETURN:
            return "RETURN";
        case AstType::LIST:
            return "LIST";
        case AstType::CLASS:
            return "CLASS";
        case AstType::IF:
            return "IF";
        case AstType::FUNCTION:
            return "FUNCTION";
        case AstType::MEMBER:
            return "MEMBER";
        case AstType::ASSIGNMENT:
            return "ASSIGNMENT";
        case AstType::BINOP:
            return "BINOP";
        case AstType::IDENTIFIER:
            return "IDENTIFIER";
        case AstType::DECLARATION:
            return "DECLARATION";
        case AstType::TYPE:
            return "TYPE";
        case AstType::NUMBER:
            return "NUMBER";
        case AstType::CALL:
            return "CALL";
        case AstType::SUB:
            return "SUBSCRIPT";
        case AstType::STRING:
            return "STRING";
        default:
            throw std::runtime_error("Unknown AstType in ast_string");

    }
}

bool compare(ListNode* a, ListNode* b) {
    if (a == nullptr and b == nullptr) {
        return true;
    }
    if (a->elements.size() != b->elements.size())return false;
    for (int i = 0; i < a->elements.size(); i++) {
        if (not equal(a->elements[i], b->elements[i])) return false;
    }
    return true;
}

bool equal(DeclarationNode* a, DeclarationNode* b) {
    if (a == nullptr and b == nullptr) {
        return true;
    }
    return (a->identifier == b->identifier) and equal(a->type, b->type) and equal(a->expression, b->expression);
}

bool compare(IdentifierNode* a, IdentifierNode* b) {
    if (a == nullptr and b == nullptr) {
        return true;
    }
    return a->name == b->name;
}

bool equal(CallNode* a, CallNode* b) {
    return equal(&a->function, &b->function) && equal(a->arguments, b->arguments);
}

bool equal(SubscriptNode* a, SubscriptNode* b) {
    return equal(a->parent, b->parent) && equal(a->sub, b->sub);
}


bool compare(BinopNode* a, BinopNode* b) {
    if (both_null(a, b)) return true;
    if (one_null(a, b)) return false;
    if (a->op != b->op) {
        return false;
    }
    if (not equal(&a->left, &b->left)) {
        return false;
    }
    if (not equal(&a->right, &b->right)) {
        return false;
    }
    return true;
}

bool equal(IfNode* a, IfNode* b) {
    if (both_null(a, b)) return true;
    if (one_null(a, b)) return false;
    if (not equal(&a->condition, &b->condition))return false;
    for (int i = 0; i < a->then.size(); i++) { if (not equal(a->then[i], b->then[i])) return false; }
    return true;
}

bool equal(FunctionNode* a, FunctionNode* b) {
    if (both_null(a, b)) return true;
    if (one_null(a, b)) return false;
    bool name_matches = a->name == b->name;
    bool return_type_matches = equal(a->return_type, b->return_type);
    bool parameter_names_match = equal(a->parameter_names, b->parameter_names);
    bool parameter_types_match = compare(a->parameter_types, b->parameter_types);
    bool body_matches = equal(a->body, b->body);
    return name_matches and return_type_matches and parameter_names_match and parameter_types_match and body_matches;
}

bool equal(ClassNode* a, ClassNode* b) {
    if (both_null(a, b)) return true;
    if (one_null(a, b)) return false;
    bool name_match = a->name == b->name;
    if (not name_match) { return false; }

    bool template_parameters_match = equal(a->template_parameters, b->template_parameters);
    if (not template_parameters_match) return false;

    bool fields_length_match = a->fields.size() == b->fields.size();
    if (not fields_length_match) { return false; }
    for (int i = 0; i < a->fields.size(); i++) {
        if (not equal(a->fields[i], b->fields[i])) { return false; }
    }

    bool methods_length_match = a->methods.size() == b->methods.size();
    if (not methods_length_match) return false;
    for (int i = 0; i < a->methods.size(); i++) {
        if (not equal(a->methods[i], b->methods[i])) { return false; }
    }

    return true;
}

bool compare(ReturnNode* a, ReturnNode* b) {
    if (both_null(a, b)) return true;
    if (one_null(a, b)) return false;
    return equal(a->expression, b->expression);
}

bool compare(AssignmentNode* a, AssignmentNode* b) {
    if (both_null(a, b)) return true;
    if (one_null(a, b)) return false;
    return equal(a->lvalue, b->lvalue) and equal(a->rvalue, b->rvalue);
}


bool equal(AstNode* a, AstNode* b) {
    if (both_null(a, b)) return true;
    if (one_null(a, b)) return false;

    if (a->type != b->type) {
        return false;
    }
    switch (a->type) {
        case AstType::ASSIGNMENT:
            return compare(a->ast_assignment, b->ast_assignment);
        case AstType::RETURN:
            return compare(a->ast_return, b->ast_return);
        case AstType::LIST:
            return compare(a->ast_list, b->ast_list);
        case AstType::CLASS:
            return equal(a->ast_class, b->ast_class);
        case AstType::IF:
            return equal(a->ast_if, b->ast_if);
        case AstType::FUNCTION:
            return equal(a->ast_function, b->ast_function);
        case AstType::MEMBER:
            return compare(a->ast_member, b->ast_member);
        case AstType::BINOP:
            return compare(a->ast_binop, b->ast_binop);
        case AstType::IDENTIFIER:
            return compare(a->ast_identifier, b->ast_identifier);
        case AstType::DECLARATION:
            return equal(a->ast_declaration, b->ast_declaration);
        case AstType::CALL:
            return equal(a->ast_call, b->ast_call);
        case AstType::NUMBER:
            return equal(a->ast_number, a->ast_number);
        case AstType::SUB:
            return equal(a->ast_sub, b->ast_sub);
        case AstType::STRING:
            return equal(a->ast_string, b->ast_string);
        default:
            throw std::runtime_error("Unknown AstType in equal: " + ast_string(a->type));

    }
}