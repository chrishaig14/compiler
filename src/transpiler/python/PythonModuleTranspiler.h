//
// Created by chris on 31/8/21.
//

#ifndef XLANG_PYTHONMODULETRANSPILER_H
#define XLANG_PYTHONMODULETRANSPILER_H

#include <simple_nodes/top/include/top.h>
#include <simple_nodes/common/include/common.h>
#include <simple_nodes/expressions/include/expressions.h>
#include <units/infos/Module.h>

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
const std::string RETURN = "return";
const std::string CLASS = "class";
const std::string RETURN_VAR = "__return__";

std::string indent_paragraph(std::string s, size_t level);

class PythonExpressionOutputCode {
public:
    PythonExpressionOutputCode(const std::string& pre_code, const std::string& code);
public:
    std::string pre_code;
    std::string code;
};

typedef std::string PythonOutputCode;

std::string pre_if_any(const PythonExpressionOutputCode& c);

class PythonModuleTranspiler {
    size_t arg_n;
public:
    bool add_self{};
    bool in_try_catch{};
    PythonExpressionOutputCode dispatch(const sem::Common& node);
    PythonOutputCode dispatch_common(const sem::Common& node);
    PythonExpressionOutputCode dispatch_expression(const sem::Exp& node, bool called_function);
    PythonOutputCode dispatch_top(const sem::Top& node);
    PythonOutputCode transpile_assignment(const sem::Assignment& node);
    PythonOutputCode transpile_block(const sem::Block& node);
    PythonExpressionOutputCode transpile_boolean(const sem::Bool& node);
    PythonOutputCode transpile_break(const sem::Break& node);
    PythonOutputCode transpile_call(const sem::Call& node);
    PythonOutputCode transpile_class(const sem::KlassDef& node);
    PythonExpressionOutputCode transpile_const_function(const sem::ConstFunction& function);
    PythonOutputCode transpile_continue(const sem::Continue& node);
    PythonOutputCode transpile_declaration(const sem::Declaration& node);
    PythonExpressionOutputCode transpile_dict(const sem::Dict& node);
    PythonExpressionOutputCode transpile_enum_member(const sem::EnumMember& node);
    PythonExpressionOutputCode transpile_float(const sem::Float& node);
    PythonOutputCode transpile_function(const sem::FunctionDef& node);
    PythonExpressionOutputCode transpile_id(const sem::Id& node);
    PythonOutputCode transpile_if(const sem::If& node);
    PythonExpressionOutputCode transpile_integer(const sem::Integer& node);
    PythonExpressionOutputCode transpile_list(const sem::List& node);
    PythonOutputCode transpile_match(const sem::Match& node);
    PythonExpressionOutputCode transpile_new(const sem::NewObject& node);
    PythonExpressionOutputCode transpile_none(const sem::None& node);
    PythonExpressionOutputCode transpile_object_constructor(const sem::ObjectConstructor& constructor);
    PythonExpressionOutputCode transpile_object_member(const sem::ObjectMember& node);
    PythonExpressionOutputCode transpile_object_method(const sem::ObjectMethod& method, bool called_function);
    PythonOutputCode transpile_return(const sem::Return& node);
    PythonExpressionOutputCode transpile_string(const sem::String& node);
    PythonExpressionOutputCode transpile_ternary(const sem::Ternary& node);
    PythonExpressionOutputCode transpile_throw(sem::Throw& node);
    PythonOutputCode transpile_try_catch(const sem::TryCatch& node);
    PythonOutputCode transpile_while(const sem::While& node);

    PythonModuleTranspiler(Module& module);
    size_t indent_level;
    size_t next_arg_n();
    std::string header;
    std::string indentation();
    std::string source;
    std::string static_cleanups;
    std::string static_initializations;
    std::string transpile_module(const sem::Module& block, Path module_path);
    Module& module;
    void indent();
    PythonOutputCode transpile_enum(const sem::EnumDef& node);
    void transpile_program(const sem::Module& node);
    void unindent();
    PythonExpressionOutputCode transpile_call_exp(const sem::CallExp& exp);
    Path module_path;
    PythonOutputCode transpile_for(const sem::For& node);
    PythonExpressionOutputCode transpile_static_method(const sem::StaticMethod& method, bool b);
    std::string clean_path(Path path);
    std::string make_full_instance_name(Path instance_path, Path class_path);
    PythonOutputCode transpile_instance(const sem::InstanceDef& def);
};


#endif //XLANG_PYTHONMODULETRANSPILER_H
