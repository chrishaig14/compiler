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
#include "../simple_nodes/BoolSNode.h"
#include "../simple_nodes/FloatSNode.h"
#include "../simple_nodes/ClassSNode.h"
#include "../simple_nodes/NewObjectSNode.h"
#include "../simple_nodes/ObjectMemberSNode.h"
#include "../simple_nodes/WhileSNode.h"
#include "../simple_nodes/ListSNode.h"
#include "../simple_nodes/IfSNode.h"
#include "../simple_nodes/BreakSNode.h"
#include "../simple_nodes/ContinueSNode.h"

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
const std::string CLASS = "class";
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

    std::string transpile_new(NewObjectSNode* node);

    std::string transpile_return(ReturnSNode* node);

    void transpile_class(ClassSNode* node);

    void dispatch_top(SNode* node) {
        switch (node->type) {
            case SNodeType::FUNCTION:
                this->transpile_function(static_cast<FunctionSNode*>(node));
                break;
            case SNodeType::CLASS:
                this->transpile_class((ClassSNode*) node);
                break;
            default:
                throw std::runtime_error("Don't know what to do with this SNode!");
        }
    }

    std::string transpile_call(CallSNode* node);

    std::string transpile_string(StringSNode* node);

    std::string transpile_boolean(BoolSNode* pNode);

    std::string transpile_float(FloatSNode* pNode);

    std::string transpile_object_member(ObjectMemberSNode* sn);

    std::string transpile_while(WhileSNode* node);

    std::string transpile_list(ListSNode* ln);

    std::string transpile_if(IfSNode* in);

    std::string transpile_break(BreakSNode* bn);

    std::string transpile_continue(ContinueSNode* pNode);

    std::string dispatch(SNode* node) {
        if (node == nullptr) {
            throw std::runtime_error("Error dispatching on nullptr snode!");
        }
        switch (node->type) {
            case SNodeType::BOOLEAN:
                return this->transpile_boolean((BoolSNode*) node);
            case SNodeType::IF:
                return this->transpile_if((IfSNode*) node);
            case SNodeType::BREAK:
                return this->transpile_break((BreakSNode*) node);
            case SNodeType::CONTINUE:
                return this->transpile_continue((ContinueSNode*) node);
            case SNodeType::ID:
                return this->transpile_id((IdSNode*) node);
            case SNodeType::STRING:
                return this->transpile_string((StringSNode*) node);
            case SNodeType::CALL:
                return this->transpile_call((CallSNode*) node);
            case SNodeType::NEW:
                return this->transpile_new((NewObjectSNode*) node);
            case SNodeType::LIST:
                return this->transpile_list((ListSNode*) node);
            case SNodeType::OBJECT_MEMBER:
                return this->transpile_object_member((ObjectMemberSNode*) node);
            case SNodeType::DECLARATION:
                return this->transpile_declaration((DeclarationSNode*) node);
            case SNodeType::WHILE:
                return this->transpile_while((WhileSNode*) node);
            case SNodeType::ASSIGNMENT:
                return this->transpile_assignment((AssignmentSNode*) node);
            case SNodeType::RETURN:
                return this->transpile_return((ReturnSNode*) node);
                break;
            case SNodeType::FLOAT:
                return this->transpile_float((FloatSNode*) node);
            case SNodeType::INTEGER:
                return this->transpile_integer((IntegerSNode*) node);
                break;
            default:
                throw std::runtime_error("Don't know what to do with this SNode!");
        }
    }
};


#endif //XLANG_STRANSPILER_H
