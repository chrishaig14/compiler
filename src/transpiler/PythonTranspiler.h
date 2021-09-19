//
// Created by chris on 31/8/21.
//

#ifndef XLANG_PYTHONTRANSPILER_H
#define XLANG_PYTHONTRANSPILER_H

#include "../simple_nodes/expressions/include/Exp.h"
#include "../simple_nodes/common/include/SNode.h"
#include "../simple_nodes/common/include/Block.h"
#include "../simple_nodes/with_unique/FunctionDef.h"
#include "../simple_nodes/with_unique/Assignment.h"
#include "../simple_nodes/common/include/Declaration.h"
#include "../simple_nodes/expressions/include/Integer.h"
#include "../simple_nodes/expressions/include/Id.h"
#include "../simple_nodes/common/include/Return.h"
#include "../simple_nodes/with_unique/EnumDef.h"
#include "../simple_nodes/common/include/Call.h"
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

    size_t next_arg_n();

    void indent();

    std::string indentation();

    void unindent();

    PythonTranspiler();

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

    PythonOutputCode dispatch_top(const sem::SNode& node);

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

    PythonOutputCode dispatch(const sem::SNode& node);

    PythonOutputCode transpile_throw(sem::Throw& node);
    bool in_try_catch;
    std::string transpile_module(const sem::Block& block);
    bool add_self;
    PythonOutputCode dispatch_expression(const sem::Exp& node);
    PythonOutputCode dispatch_common(const sem::SNode& node);
};


#endif //XLANG_PYTHONTRANSPILER_H
