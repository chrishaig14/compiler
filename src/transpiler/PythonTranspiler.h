//
// Created by chris on 31/8/21.
//

#ifndef XLANG_PYTHONTRANSPILER_H
#define XLANG_PYTHONTRANSPILER_H

#include "../simple_nodes/common/include/Block.h"
#include "../simple_nodes/with_unique/FunctionDef.h"
#include "../simple_nodes/with_unique/Assignment.h"
#include "../simple_nodes/common/include/Declaration.h"
#include "../simple_nodes/expressions/include/Integer.h"
#include "../simple_nodes/expressions/include/Id.h"
#include "../simple_nodes/with_unique/Return.h"
#include "../simple_nodes/with_unique/EnumDef.h"
#include "../simple_nodes/with_unique/Call.h"
#include "../simple_nodes/expressions/include/String.h"
#include "../simple_nodes/expressions/include/Bool.h"
#include "../simple_nodes/expressions/include/Float.h"
#include "../simple_nodes/common/include/KlassDef.h"
#include "../simple_nodes/common/include/NewObject.h"
#include "../simple_nodes/common/include/Match.h"
#include "../simple_nodes/expressions/include/ObjectMember.h"
#include "../simple_nodes/with_unique/While.h"
#include "../simple_nodes/expressions/include/List.h"
#include "../simple_nodes/with_unique/IfSNode.h"
#include "../simple_nodes/with_unique/Break.h"
#include "../simple_nodes/with_unique/Continue.h"
#include "../simple_nodes/expressions/include/EnumMember.h"
#include "../simple_nodes/expressions/include/Ternary.h"
#include "../simple_nodes/expressions/include/None.h"
#include "../simple_nodes/expressions/include/Dict.h"
#include "../simple_nodes/common/include/Throw.h"
#include "../simple_nodes/common/include/TryCatch.h"

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

std::string indent_paragraph(std::string s, size_t level);

class PythonOutputCode {
public:
    PythonOutputCode(const std::string& pre_code, const std::string& code);
public:
    std::string pre_code;
    std::string code;
};

class PythonTranspiler {
    size_t arg_n;
public:
    std::string source;
    std::string header;
    std::string static_initializations;
    std::string static_cleanups;
    size_t indent_level;

    size_t next_arg_n() {
        return this->arg_n++;
    }

    void indent() {
        this->indent_level += 4;
    }

    std::string indentation() {
        return std::string(this->indent_level, ' ');
    }

    void unindent() {
        this->indent_level -= 4;
    }

    PythonTranspiler() {
        this->indent_level = 0;
        this->arg_n = 0;
    }

    PythonOutputCode transpile_integer(const sem::Integer& node);

    PythonOutputCode transpile_function(const sem::FunctionDef& node);

    PythonOutputCode transpile_declaration(const sem::Declaration& node);

    PythonOutputCode transpile_assignment(const sem::Assignment& node);
    PythonOutputCode transpile_block(const sem::Block& node);
    void transpile_program(const sem::Block& node);
    PythonOutputCode transpile_id(const sem::Id& node);

    PythonOutputCode transpile_new(const sem::NewObject& node);

    PythonOutputCode transpile_return(const sem::Return& node);

    PythonOutputCode transpile_class(const sem::KlassDef& node);
    void transpile_enum(const sem::EnumDef& node);

    PythonOutputCode dispatch_top(const sem::SNode& node) {
        switch (node.type) {
            case SNodeType::FUNCTION:
                return this->transpile_function((const sem::FunctionDef&) (node));
                break;
                // case SNodeType::ENUM:
                //     return this->transpile_enum((const sem::EnumDef&) node);
                //     break;
            case SNodeType::CLASS:
                return this->transpile_class((const sem::KlassDef&) node);
                break;
            default:
                throw std::runtime_error("Don't know what to do with this SNode!");
        }
    }

    PythonOutputCode transpile_call(const sem::Call& node);

    PythonOutputCode transpile_string(const sem::String& node);

    PythonOutputCode transpile_boolean(const sem::Bool& node);

    PythonOutputCode transpile_float(const sem::Float& node);

    PythonOutputCode transpile_object_member(const sem::ObjectMember& node);

    PythonOutputCode transpile_while(const sem::While& node);

    PythonOutputCode transpile_list(const sem::List& node);

    PythonOutputCode transpile_if(const sem::IfSNode& node);

    PythonOutputCode transpile_break(const sem::Break& node);

    PythonOutputCode transpile_continue(const sem::Continue& node);

    PythonOutputCode transpile_match(const sem::Match& node);

    PythonOutputCode transpile_enum_member(const sem::EnumMember& node);

    PythonOutputCode transpile_ternary(const sem::Ternary& node);

    PythonOutputCode transpile_none(const sem::None& node);

    PythonOutputCode transpile_dict(const sem::Dict& node);
    PythonOutputCode transpile_try_catch(const sem::TryCatch& node);

    PythonOutputCode transpile_const_function_call(const sem::ConstFunctionCall& call);

    PythonOutputCode dispatch(const sem::SNode& node) {
        switch (node.type) {
            case SNodeType::BLOCK:
                return this->transpile_block((const sem::Block&) node);
            case SNodeType::ENUM_MEMBER:
                return this->transpile_enum_member((const sem::EnumMember&) node);
            case SNodeType::BOOLEAN:
                return this->transpile_boolean((const sem::Bool&) node);
            case SNodeType::MATCH:
                return this->transpile_match((const sem::Match&) node);
            case SNodeType::IF:
                return this->transpile_if((const sem::IfSNode&) node);
            case SNodeType::BREAK:
                return this->transpile_break((const sem::Break&) node);
            case SNodeType::CONTINUE:
                return this->transpile_continue((const sem::Continue&) node);
            case SNodeType::ID:
                return this->transpile_id((const sem::Id&) node);
            case SNodeType::STRING:
                return this->transpile_string((const sem::String&) node);
            case SNodeType::CALL:
                return this->transpile_call((const sem::Call&) node);
            case SNodeType::OBJECT_METHOD_CALL:
                return this->transpile_object_method_call((const sem::ObjectMethodCall&) node);
            case SNodeType::CONST_FUNCTION_CALL:
                return this->transpile_const_function_call((const sem::ConstFunctionCall&) node);
            case SNodeType::NEW:
                return this->transpile_new((const sem::NewObject&) node);
            case SNodeType::DICT:
                return this->transpile_dict((const sem::Dict&) node);
            case SNodeType::LIST:
                return this->transpile_list((const sem::List&) node);
            case SNodeType::OBJECT_MEMBER:
                return this->transpile_object_member((const sem::ObjectMember&) node);
            case SNodeType::DECLARATION:
                return this->transpile_declaration((const sem::Declaration&) node);
            case SNodeType::WHILE:
                return this->transpile_while((const sem::While&) node);
            case SNodeType::ASSIGNMENT:
                return this->transpile_assignment((const sem::Assignment&) node);
            case SNodeType::RETURN:
                return this->transpile_return((const sem::Return&) node);
            case SNodeType::TRY_CATCH:
                return this->transpile_try_catch((const sem::TryCatch&) node);
                // case SNodeType::THROW:
                //     return this->transpile_throw((const sem::Throw&) node);
                //     break;
            case SNodeType::FLOAT:
                return this->transpile_float((const sem::Float&) node);
            case SNodeType::NONE:
                return this->transpile_none((const sem::None&) node);
            case SNodeType::TERNARY:
                return this->transpile_ternary((const sem::Ternary&) node);
            case SNodeType::INTEGER:
                return this->transpile_integer((const sem::Integer&) node);
                break;
            default:
                throw std::runtime_error("Don't know what to do with this SNode!");
        }
    }

    PythonOutputCode transpile_throw(sem::Throw& node);
    bool in_try_catch;
    PythonOutputCode transpile_object_method_call(const sem::ObjectMethodCall& call);
    std::string transpile_module(const sem::Block& block);
    bool add_self;
};


#endif //XLANG_PYTHONTRANSPILER_H
