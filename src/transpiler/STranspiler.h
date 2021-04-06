//
// Created by chris on 4/4/21.
//

#ifndef XLANG_STRANSPILER_H
#define XLANG_STRANSPILER_H

#include "../simple_nodes/BlockSNode.h"
#include "../simple_nodes/FunctionSNode.h"
#include "../simple_nodes/AssignmentSNode.h"
#include "../simple_nodes/DeclarationSNode.h"
#include "../simple_nodes/IntegerSNode.h"
#include "../simple_nodes/IdSNode.h"
#include "../simple_nodes/ReturnSNode.h"
#include "../simple_nodes/CallSNode.h"
#include "../simple_nodes/StringSNode.h"

const std::string TOBJECT = "TaggedObject*";
const std::string GCDECLARE = "GC::declare";
const std::string GCASSIGN = "GC::assign";
const std::string GCRETURN = "GC::set_return";
const std::string GCOUTOFSCOPE = "GC::out_of_scope";
const std::string LPAREN = "(";
const std::string RPAREN = ")";
const std::string SEMIC = ";";
const std::string LCURLY = "{";
const std::string RCURLY = "}";
const std::string ASSIGN = "=";
const std::string QUOTE = "\"";
const std::string SPACE = " ";
const std::string COMMA = ",";
const std::string NEWLINE = "\n";
const std::string EXTERN = "extern";
const std::string RETURN = "return";
const std::string RETURN_VAR = "__return__";


#define GASSIGN(exp, new_value) "GC::assign(" exp, new_value ")"

class STranspiler {

public:
    std::string source;
    std::string header;
    std::string static_initializations;
    std::string static_cleanups;

    std::string transpile_integer(IntegerSNode* node);

    void transpile_function(FunctionSNode* node);

    std::string transpile_declaration(DeclarationSNode* node);

    std::string transpile_assignment(AssignmentSNode* node);
    std::string transpile_block(BlockSNode* node);
    void transpile_program(BlockSNode* node);
    std::string transpile_id(IdSNode* node);
    std::string transpile_return(ReturnSNode* node);

    void dispatch_top(SNode* node) {
        switch (node->type) {
            case SNodeType::FUNCTION:
                this->transpile_function(static_cast<FunctionSNode*>(node));
                break;
        }
    }

    std::string transpile_call(CallSNode* node);

    std::string transpile_string(StringSNode* node);

    std::string dispatch(SNode* node) {
        switch (node->type) {
            case SNodeType::ID:
                return this->transpile_id((IdSNode*) node);
            case SNodeType::STRING:
                return this->transpile_string((StringSNode*) node);
            case SNodeType::CALL:
                return this->transpile_call((CallSNode*) node);
            case SNodeType::DECLARATION:
                return this->transpile_declaration((DeclarationSNode*) node);
            case SNodeType::ASSIGNMENT:
                return this->transpile_assignment((AssignmentSNode*) node);
            case SNodeType::RETURN:
                return this->transpile_return((ReturnSNode*) node);
                break;
            case SNodeType::INTEGER:
                return this->transpile_integer((IntegerSNode*) node);
                break;
        }
    }
};


#endif //XLANG_STRANSPILER_H
