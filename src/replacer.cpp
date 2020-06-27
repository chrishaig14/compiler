//
// Created by chris on 15/6/20.
//

#include "replacer.h"


void replace_types(TypeNode* node, VectorOfStrings params, VectorOfTypes replacements) {
    if (node->type_parameters.size() == 0) {
        for (int i = 0; i < params.size(); i++) {
            if (node->name == params[i]) {
                node->name = replacements[i]->name;
                node->type_parameters = replacements[i]->type_parameters;
                break;
            }
        }
    } else {
        for (int i = 0; i < node->type_parameters.size(); i++) {
            replace_types(node->type_parameters[i], params, replacements);
        }
    }
}

void replace_types(DeclarationNode* node, VectorOfStrings params, VectorOfTypes replacements) {
    replace_types(node->type, params, replacements);
}

void replace_types(FunctionNode* node, VectorOfStrings params, VectorOfTypes replacements) {
    for (int i = 0; i < node->parameter_types.size(); i++) {
        replace_types(node->parameter_types[i], params, replacements);
    }
    replace_types(node->return_type, params, replacements);
    replace_types(node->body, params, replacements);
}

void replace_types(VectorOfNodes block, VectorOfStrings params, VectorOfTypes replacements) {
    VectorOfNodes new_block;
    for (int i = 0; i < block.size(); i++) {
        replace_types(block[i], params, replacements);
    }
}

void replace_types(AstNode* node, VectorOfStrings params, VectorOfTypes replacements) {
    switch (node->type) {
        case AstType::ASSIGNMENT:
            break;
        case AstType::RETURN:
            break;
        case AstType::LIST:
            break;
        case AstType::CLASS:
            replace_types(node->ast_class, params, replacements);
            break;
        case AstType::IF:
            replace_types(node->ast_if->then, params, replacements);
            break;
        case AstType::FUNCTION:
            replace_types(node->ast_function, params, replacements);
            break;
        case AstType::MEMBER:
            break;
        case AstType::BINOP:
            break;
        case AstType::IDENTIFIER:
            break;
        case AstType::DECLARATION:
            replace_types(node->ast_declaration, params, replacements);
            break;
    }
}

void replace_types(ClassNode* node, VectorOfStrings params, VectorOfTypes replacements) {}