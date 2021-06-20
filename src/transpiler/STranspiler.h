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
#include "../simple_nodes/EnumSNode.h"
#include "../simple_nodes/CallSNode.h"
#include "../simple_nodes/StringSNode.h"
#include "../simple_nodes/BoolSNode.h"
#include "../simple_nodes/FloatSNode.h"
#include "../simple_nodes/ClassSNode.h"
#include "../simple_nodes/NewObjectSNode.h"
#include "../simple_nodes/MatchSNode.h"
#include "../simple_nodes/ObjectMemberSNode.h"
#include "../simple_nodes/WhileSNode.h"
#include "../simple_nodes/ListSNode.h"
#include "../simple_nodes/IfSNode.h"
#include "../simple_nodes/BreakSNode.h"
#include "../simple_nodes/ContinueSNode.h"
#include "../simple_nodes/EnumMemberSNode.h"
#include "../simple_nodes/TernarySNode.h"
#include "../simple_nodes/NoneSNode.h"
#include "../simple_nodes/DictSNode.h"
#include "../simple_nodes/ThrowSNode.h"
#include "../simple_nodes/TryCatchSNode.h"

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

class OutputCode {
public:
    OutputCode(const std::string& pre_code, const std::string& code);
public:
    std::string pre_code;
    std::string code;

};

class STranspiler {

public:
    std::string source;
    std::string header;
    std::string static_initializations;
    std::string static_cleanups;

    OutputCode transpile_integer(IntegerSNode* node);

    void transpile_function(FunctionSNode* node);

    OutputCode transpile_declaration(DeclarationSNode* node);

    OutputCode transpile_assignment(AssignmentSNode* node);
    OutputCode transpile_block(BlockSNode* node);
    void transpile_program(BlockSNode* node);
    OutputCode transpile_id(IdSNode* node);

    OutputCode transpile_new(NewObjectSNode* node);

    OutputCode transpile_return(ReturnSNode* node);

    void transpile_class(ClassSNode* node);
    void transpile_enum(EnumSNode* node);

    void dispatch_top(SNode* node) {
        switch (node->type) {
            case SNodeType::FUNCTION:
                this->transpile_function(static_cast<FunctionSNode*>(node));
                break;
            case SNodeType::ENUM:
                this->transpile_enum((EnumSNode*) node);
                break;
            case SNodeType::CLASS:
                this->transpile_class((ClassSNode*) node);
                break;
            default:
                throw std::runtime_error("Don't know what to do with this SNode!");
        }
    }

    OutputCode transpile_call(CallSNode* node);

    OutputCode transpile_string(StringSNode* node);

    OutputCode transpile_boolean(BoolSNode* pNode);

    OutputCode transpile_float(FloatSNode* pNode);

    OutputCode transpile_object_member(ObjectMemberSNode* sn);

    OutputCode transpile_while(WhileSNode* node);

    OutputCode transpile_list(ListSNode* ln);

    OutputCode transpile_if(IfSNode* in);

    OutputCode transpile_break(BreakSNode* bn);

    OutputCode transpile_continue(ContinueSNode* pNode);

    OutputCode transpile_match(MatchSNode* mn);

    OutputCode transpile_enum_member(EnumMemberSNode* emsn);

    OutputCode transpile_ternary(TernarySNode* tn);

    OutputCode transpile_none(NoneSNode* nn);

    OutputCode transpile_dict(DictSNode* dn);
    OutputCode transpile_try_catch(TryCatchSNode* dn);

    OutputCode dispatch(SNode* node) {
        if (node == nullptr) {
            throw std::runtime_error("Error dispatching on nullptr snode!");
        }
        switch (node->type) {
            case SNodeType::BLOCK:
                return this->transpile_block((BlockSNode*) node);
            case SNodeType::ENUM_MEMBER:
                return this->transpile_enum_member((EnumMemberSNode*) node);
            case SNodeType::BOOLEAN:
                return this->transpile_boolean((BoolSNode*) node);
            case SNodeType::MATCH:
                return this->transpile_match((MatchSNode*) node);
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
            case SNodeType::DICT:
                return this->transpile_dict((DictSNode*) node);
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
            case SNodeType::TRY_CATCH:
                return this->transpile_try_catch((TryCatchSNode*) node);
            case SNodeType::THROW:
                return this->transpile_throw((ThrowSNode*) node);
                break;
            case SNodeType::FLOAT:
                return this->transpile_float((FloatSNode*) node);
            case SNodeType::NONE:
                return this->transpile_none((NoneSNode*) node);
            case SNodeType::TERNARY:
                return this->transpile_ternary((TernarySNode*) node);
            case SNodeType::INTEGER:
                return this->transpile_integer((IntegerSNode*) node);
                break;
            default:
                throw std::runtime_error("Don't know what to do with this SNode!");
        }
    }

    OutputCode transpile_throw(ThrowSNode* node);
    bool in_try_catch;
};


#endif //XLANG_STRANSPILER_H
