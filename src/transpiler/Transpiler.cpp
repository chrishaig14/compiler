//
// Created by chris on 2/8/20.
//

#include <iostream>
#include "Transpiler.h"
#include "../macros.h"
#include "../semantic/Checker.h"

#define FUNCTION_PREFIX "function_"

void Transpiler::transpile(BlockNode* node) {
    m_source += "#include \"" + this->current_module + ".h\"\n";
    this->dispatch(node);
    m_header += "#endif //" + this->current_module;

}

inline std::string get_class_name(const std::string& c) {
    return "class_" + c;
}

std::string Transpiler::visit_assignment(AssignmentNode& node) {
    this->is_lvalue = true;
    std::string out;
    if (node.lvalue->ntype == SUB) {
        out += "set_subscript(" + this->dispatch(node.lvalue->sub().parent) + ", " +
               this->dispatch(node.lvalue->sub().child[0]) + ", " + this->dispatch(node.rvalue) + ")";
    } else if (node.lvalue->ntype == MEMBER) {
        this->is_lvalue = false;
        MemberNode& memberNode = node.lvalue->member();
        TypeNode* cast_type = memberNode.parent_t;
        std::string mem =
                "CAST(" + this->dispatch(node.lvalue->member().parent) + "," + this->map[cast_type->object().id] +
                ")->" + node.lvalue->member().s_child;
        out += mem + "=GC::assign(" + mem + ",";
        out += this->dispatch(node.rvalue) + ");";
    } else {
        if (is_object(*node.type)) {
            out += this->dispatch(node.lvalue) + " = GC::assign(";
            out += this->dispatch(node.lvalue) + ", ";
            this->is_lvalue = false;
            std::string var_type = this->type_mapper(*node.type);
            out += this->dispatch(node.rvalue) + ");";
        } else {
            out += this->dispatch(node.lvalue) + " = ";
            this->is_lvalue = false;
            std::string var_type = this->type_mapper(*node.type);
            out += this->dispatch(node.rvalue) + ";";
        }

    }
    return out;
}

std::string Transpiler::visit_binop(BinopNode& node) {
    std::string prefix;
    if (*node.ltype == T_INT) {
        prefix = "INT";
    } else if (*node.ltype == T_FLOAT) {
        prefix = "FLOAT";
    } else if (*node.ltype == T_DOUBLE) {
        prefix = "DOUBLE";
    }
    std::map<OpType, std::string> ops;
    ops[OpType::ADD] = "ADD";
    ops[OpType::SUB] = "SUB";
    ops[OpType::MUL] = "MUL";
    ops[OpType::DIV] = "DIV";
    ops[OpType::MOD] = "MOD";
    std::string out =
            prefix + "_" + ops[node.op] + "(" + this->dispatch(node.left) + ", " + this->dispatch(node.right) + ")";
    return out;
}

std::string Transpiler::visit_block(BlockNode& node) {
    std::string out;
    for (auto n: node.nodes) {
        out += this->dispatch(n);
        if (n->ntype != FUNC && n->ntype != WHIL && n->ntype != IFF && n->ntype != CLS) {
            out += ";\n";
        }
    }
    if (node.nodes.back()->ntype != RETRN) {
        for (auto v: node.local_vars) {
            if (v.first == "this") {
                continue;
            }
            if (is_object(*v.second)) {
                out += "GC::out_of_scope(" + (v.first == "this" ? "this_obj" : v.first) + ");\n";
            }
        }
    }
    return out;
}

std::string Transpiler::visit_boolean(BooleanNode& node) {
    return node.value ? "TRUE" : "FALSE";
}

std::string Transpiler::visit_bool_op(BoolOpNode& node) {
    std::string out = "";
    std::string op;
    switch (node.op) {
        case BoolOp::EQ:
            op += "eq";
            break;
        case BoolOp::AND:
            op += "&&";
            break;
        case BoolOp::OR:
            op += "||";
            break;
        case BoolOp::LEQ:
            op += "<=";
            break;
        case BoolOp::GEQ:
            op += ">=";
            break;
        case BoolOp::LT:
            op += "<";
            break;
        case BoolOp::GT:
            op += ">";
            break;
        case BoolOp::NEQ:
            op += "__neq__";
            break;
    }
    std::string eq_method_name = "eq";
    if (op == eq_method_name || op == "__neq__") {
        if (is_generic(*node.ltype)) {
            return "EQ(" + this->dispatch(node.left) + "," + this->dispatch(node.right) + ")";
        } else {
            return "CALL2(function_" + node.ltype->object().id + "_" + eq_method_name + "," +
                   this->dispatch(node.left) + ", " + this->dispatch(node.right) + ")";
        }
    }
    if (node.op == BoolOp::AND || node.op == BoolOp::OR) {
        return "MAKE_BOOL(GET_BOOL(" + this->dispatch(node.left) + ")" + op + "GET_BOOL(" + this->dispatch(node.right) +
               "))";
    }
    return "MAKE_BOOL(GET_INT(" + this->dispatch(node.left) + ")" + op + "GET_INT(" + this->dispatch(node.right) + "))";
}

std::string Transpiler::visit_break(BreakNode& node) {
    std::string out;
    for (auto v: node.loop_vars) {
        if (is_object(*v.second)) {
            out += "GC::out_of_scope(" + (v.first == "this" ? "this_obj" : v.first) + ");\n";
        }
    }
    out += "break";
    return out;
}

bool is_object(const TypeNode& t) {
    if (t.kind == Kind::OBJECT) {
        std::string id = t.object().id;
        if (id == "Integer" || id == "Boolean") {
            return false;
        }
    }
    return true;
}

std::string Transpiler::visit_call(CallNode& node) {
    std::string out;
    out += "CALL" + std::to_string(node.arguments.size()) + "(";
    bool old_is_call = this->is_call;
    this->is_call = true;
    out += this->dispatch(node.function) + ", ";
    this->is_call = old_is_call;
    for (int i = 0; i < node.arguments.size(); i++) {
        std::string w = this->dispatch(node.arguments[i]);
        out += w + ", ";
    }
    out = out.substr(0, out.size() - 2);
    out += ")";
    return out;
}

std::string generate_destructor(std::string class_name, VectorOfStrings members) {
    std::string destructor;
    destructor += class_name + "::~" + class_name + "(){\n";
    for (auto m: members) {
        destructor += "GC::out_of_scope(this->" + m + ");\n";
    }
    destructor += "}";
    return destructor;
}

std::string generate_destructor_header(std::string class_name, VectorOfStrings members) {
    std::string destructor;
    destructor += "~" + class_name + "() override;\n";
    return destructor;
}


std::string generate_constructor_header(std::string class_name, VectorOfStrings members) {
    std::string out = class_name + "(";
    for (int i = 0; i < members.size(); i++) {
        out += "TaggedObject* " + members[i] + ", ";
    }
    out = out.substr(0, out.size() - 2);
    out += ");\n";
    return out;
}

std::string generate_constructor(std::string class_name, VectorOfStrings members) {
    std::string out = class_name + "::" + class_name + "(";
    for (int i = 0; i < members.size(); i++) {
        out += "TaggedObject* " + members[i] + ", ";
    }
    out = out.substr(0, out.size() - 2);
    out += "):";
    out += "XObject(\"" + class_name + "\")";
    out += "{\n";
    for (int i = 0; i < members.size(); i++) {
        out += "this->" + members[i] + " = " + members[i] + ";\n";
    }
    out += "}\n";
    return out;
}

std::string Transpiler::visit_class(ClassNode& node) {
    std::string mangled_name = this->map[node.class_name];
    this->method_class = node.class_name;
    this->num_members_class = node.members.size();
    std::string class_name = mangled_name;
    m_header += "class " + class_name + " : public XObject {\n";
    std::sort(node.members_ordered.begin(), node.members_ordered.end());
    m_header += "public:\n";
    for (int i = 0; i < node.members_ordered.size(); i++) {
        m_header += "TaggedObject* " + node.members_ordered[i] + ";\n";
    }
    m_header += "\n";
    m_header += generate_constructor_header(class_name, node.members_ordered);
    m_header += generate_destructor_header(class_name, node.members_ordered);
    m_source += generate_constructor(class_name, node.members_ordered);
    m_source += generate_destructor(class_name, node.members_ordered);
    m_header += "};\n";
    for (auto n: node.methods) {
        std::string method_name = n.second->identifier;
        n.second->identifier = node.class_name + "." + n.second->identifier;
        if (method_name != "init") {
            n.second->parameter_names.insert(n.second->parameter_names.begin(), "this_obj");
            n.second->parameter_types.insert(n.second->parameter_types.begin(), new ObjectType(this->method_class));
        }
        m_source += this->dispatch(n.second);
    }
    return "ASDASDFSDF";
}

std::string Transpiler::ptr_to_type_object(const ObjectType& t) {
    if (t.id.size() == 1 && islower(t.id[0])) {
        // it's a generic, return same withsource pointer;
        return "void*";
    }
    if (t == T_INT) {
        return "void*";
    }
    if (t == T_STRING) {
        return "void*";
    }
    if (t == T_BOOL) {
        return "bool";
    }
    if (t.id == "Tuple") {
        for (int i = 0; i < this->tuple_types.size(); i++) {
            if (t == *this->tuple_types[i]) {
                return "XTuple" + std::to_string(i) + "*";
            }
        }
    }
    if (t.id == "Option") {
        return this->type_mapper(*t.type_params[0]);
    }
    if (t.id == "List") {
        return "PTR_TO_LIST";
    }
    return this->map[t.id] + "*";
}


std::string Transpiler::object_type_mapper(const ObjectType& t) {
    if (t.id.size() == 1 && islower(t.id[0])) {
        // it's a generic, return same withsource pointer;
        return "aXObject*";
    }
    if (t == T_INT) {
        return "TaggedObject*";
    }
    if (t == T_STRING) {
        return "XString";
    }
    if (t == T_BOOL) {
        return "TaggedObject*";
    }
    if (t.id == "Tuple") {
        return "XTuple" + std::to_string(t.type_params.size());
    }
    if (t.id == "Option") {
        return this->type_mapper(*t.type_params[0]);
    }
    if (t.id == "List") {
        return "XList*";
    }
    return this->map[t.id] + "";
}

std::string Transpiler::add_type(const TypeNode& t, std::string n) {
    if (t.kind == Kind::OBJECT) {
        return this->type_mapper(t.object()) + " " + n;
    }
    return this->wrap_in_function_type(t.function(), n);
}

std::string Transpiler::wrap_in_function_type(const FunctionType& t, std::string n) {
    std::string out;
    out = "Function" + std::to_string(t.param_types.size()) + "<";
    for (int i = 0; i < t.param_types.size(); i++) {
        out += this->type_mapper(*t.param_types[i]) + ", ";
    }
    out += this->type_mapper(*t.return_type);
    out += ">*";
    out += " " + n;
    return out;
}

std::string Transpiler::function_type_mapper(const FunctionType& t) {
    std::string out;
    out += "Function" + std::to_string(t.param_types.size()) + "*";
    return out;
}

std::string Transpiler::ptr_to_type(const TypeNode& t) {
    if (t.kind == Kind::OBJECT) {
        return this->ptr_to_type_object(t.object());
    }
    return this->function_type_mapper(t.function());
}


std::string Transpiler::type_mapper(const TypeNode& t) {
    if (t.kind == Kind::OBJECT) {
        return this->object_type_mapper(t.object());
    }
    return this->function_type_mapper(t.function());
}

std::string Transpiler::visit_continue(ContinueNode& node) {
    return "";
}

std::string Transpiler::visit_declaration(DeclarationNode& node) {
    std::string out;
    for (int i = 0; i < node.identifier.size(); i++) {
        if (node.identifier[i] == '.') {
            node.identifier[i] = '_';
        }
    }
    if (node.expression->ntype == TERNARY) {
        out += "it = " + this->dispatch(node.expression->ternary().expression) + ";\n";
    }
    std::string var_type = this->type_mapper(*node.type);
    out += "TaggedObject* " + node.identifier + " = ";
    if (is_object(*node.type)) {
        out += "GC::declare(" + this->dispatch(node.expression) + ")";
    } else {
        out += this->dispatch(node.expression);
    }
    return out;
}

std::string Transpiler::visit_empty_list(EmptyListNode& node) {
    return "LIST()";
}

std::string Transpiler::visit_number(NumberNode& node) {
    std::string t;
    switch (node.num_type) {
        case NumberType::INTEGER:
            t = "INT";
            break;
        case NumberType::FLOAT:
            t = "FLOAT";
            break;
        case NumberType::DOUBLE:
            t = "DOUBLE";
            break;
    }
    return "MAKE_" + t + "(" + node.str + ")";
}

std::string Transpiler::visit_for(ForNode& node) {
    std::string out;
    out += "TaggedObject* _for_list = " + this->dispatch(node.exp) + ";\n";
    out += "for(int myindex=0;myindex < ((XList*)(UNTAG(_for_list)))->l->size(); myindex++){\n";
    out += "TaggedObject* " + node.var + " = LIST_SUBSCRIPT(_for_list,MAKE_INT(myindex));\n";
    out += this->dispatch(node.body);
    out += "}";
    return out;
}

std::set<std::string> get_generic_types(const TypeNode& t) {
    std::set<std::string> types;
    if (t.kind == Kind::OBJECT) {
        if (t.object().type_params.size() == 0) {
            types.insert(get_class_name(t.object().id));
            return types;
        }
        for (auto tp: t.object().type_params) {
            if (is_generic(*tp)) {
                for (auto pp: get_generic_types(*tp)) {
                    types.insert(pp);
                }
            }
        }
    } else {
        for (auto tp: t.function().param_types) {
            if (is_generic(*tp)) {
                for (auto pp: get_generic_types(*tp)) {
                    types.insert(pp);
                }
            }
        }
        if (is_generic(*t.function().return_type)) {
            for (auto pp: get_generic_types(*t.function().return_type)) {
                types.insert(pp);
            }
        }
    }
    return types;
}

std::string Transpiler::visit_function(FunctionNode& node) {
    std::string fname = this->map.at(node.identifier);
    std::string raw_function_name = fname;

    std::string signature;

    signature += "TaggedObject*";
    signature += " ";
    signature += raw_function_name + "_f";
    signature += "(";
    for (int i = 0; i < node.parameter_types.size(); i++) {
        signature += std::string() + "TaggedObject*" + " " + node.parameter_names[i] + ", ";
    }
    if (node.parameter_types.size() > 0) {
        signature = signature.substr(0, signature.size() - 2);
    }
    signature += ")";
    std::string f_source = signature;
    this->m_header += signature + ";\n";
    f_source += "{\n";
    for (int i = 0; i < node.parameter_types.size(); i++) {
        if (is_object(*node.parameter_types[i])) {
            f_source += "GC::declare(" + node.parameter_names[i] + ");\n";
        } else {
            f_source += node.parameter_names[i] + ";\n";
        }
    }

    f_source += "void* it = nullptr;\n";
    bool is_init = false;
    if (node.identifier == this->method_class + ".init") {
        is_init = true;
        f_source += "TaggedObject* this_obj = NEW(";
        f_source += this->map[this->method_class] + ",";
        for (int i = 0; i < this->num_members_class; i++) {
            f_source += "nullptr, ";
        }
        if (this->num_members_class != 0) {
            f_source = f_source.substr(0, f_source.size() - 2);
        }
        f_source += ");\n";

    }
    f_source += this->dispatch(node.body);
    if (is_init) {
        f_source += "return this_obj;\n";
    } else if (node.return_type->kind == Kind::OBJECT && node.return_type->object().id == ".None") {
        f_source += "return nullptr;\n";
    }
    f_source += "}\n";
    std::string num_args_str = std::to_string(node.parameter_names.size());
    std::string function_obj_name = fname;

    std::string function_class = "Function" + num_args_str;
    this->m_header += "extern TaggedObject* " + function_obj_name + ";\n";
    // this->static_declarations += "static Function" + num_args_str + "* " + function_obj_name + ";\n";
    this->m_source +=
            function_class + " " + raw_function_name + "_o" + " = " + function_class + " (" + raw_function_name + "_f"
                                                                                                                  ");\n";
    this->m_source += "TaggedObject* " + function_obj_name + "=FTAG(&" + raw_function_name + "_o);\n";
    this->m_source += f_source;

    return "";
}

std::string Transpiler::visit_id(IdNode& node) {
    std::string idn = node._id;
    if (node.is_global_function) {
        return this->map.at(node._id);
    }
    if (idn == "this") {
        idn = "this_obj";
    }

    std::string out = idn;
    for (int i = 0; i < out.size(); i++) {
        if (out[i] == '.') {
            out[i] = '_';
        }
    }
    return out;
}

std::string Transpiler::visit_if(IfNode& node) {
    std::string out;
    out = "if";
    out += "(GET_BOOL(" + this->dispatch(node.condition) + "))" + "{";
    out += this->visit_block(*node.then);
    out += "}";
    for (int i = 0; i < node.elifs.size(); i++) {
        out += "else if (GET_BOOL(" + this->dispatch(node.elifs[i].first) + ")){" +
               this->visit_block(*node.elifs[i].second) + "}";
    }
    if (node.selse != nullptr) {
        out += "else {" + this->dispatch(node.selse) + "}";
    }
    return out;
}

std::string Transpiler::visit_list(ListNode& node) {
    std::string out;
    out = "LIST(";
    for (int i = 0; i < node.elements.size(); i++) {
        std::string w = this->dispatch(node.elements[i]);
        if (node.elements[i]->ntype != ID) {
        }
        out += w + ", ";
    }
    if (node.elements.size() != 0) {
        out = out.substr(0, out.size() - 2);
    }
    out += ")";
    return out;
}

std::string Transpiler::visit_dict(DictNode& node) {
    std::string out;
    out = "NEW(XDict,std::unordered_map<TaggedObject*,TaggedObject*>({";
    for (int i = 0; i < node.items.size(); i++) {
        out += "{" + this->dispatch(node.items[i].first) + ", " + this->dispatch(node.items[i].second) + "}, ";
    }
    if (node.items.size() != 0) {
        out = out.substr(0, out.size() - 2);
    }
    out += "}))";
    return out;
}

std::string Transpiler::visit_member(MemberNode& node) {
    if (node.type == MemberType::NUM) {
        std::string s = "CAST(" + this->dispatch(node.parent) + "," + this->type_mapper(*node.parent_t) + ")->mem_" +
                        std::to_string(node.n_child) + "";
        return s;
    }
    auto s = "CAST(" + this->dispatch(node.parent) + "," + this->map[node.parent_t->object().id] + ")->" + node.s_child;
    return s;
}

std::string Transpiler::visit_none(NoneNode& node) {
    return "nullptr";
}

std::string Transpiler::visit_return(ReturnNode& node) {
    std::string out;
    if (node.expression != nullptr) {
        if (is_object(*node.ret_type)) {
            out += "TaggedObject* __return__ = GC::set_return(" + this->dispatch(node.expression) + ");\n";
        } else {
            out += "TaggedObject* __return__ = " + this->dispatch(node.expression) + ";\n";
        }
    } else {
        out += "TaggedObject* __return__ = nullptr";
    }
    for (auto v: node.reachables) {
        if (is_object(*v.second)) {
            out += "GC::out_of_scope(" + (v.first == "this" ? "this_obj" : v.first) + ");\n";
        }
    }
    out += "return __return__";
    return out;

}

std::string Transpiler::visit_string(StringNode& node) {
    std::string out;
    out += "MAKE_STRING(\"" + node.str + "\")";
    return out;
}

std::string Transpiler::visit_subscript(SubscriptNode& node) {
    std::string out;
    std::string fun;
    if (node.parent_t->object().id == "List") {
        out = "LIST_SUBSCRIPT(" + this->dispatch(node.parent) + ", " + this->dispatch(node.child[0]) + ")";
    } else if (node.parent_t->object().id == "String") {
        fun = "function_string_subscript";
        out = "CALL2(" + fun + "," + this->dispatch(node.parent) + ", " + this->dispatch(node.child[0]) + ")";
    } else {
        fun = "function_dict_subscript";
        out = "CALL2(" + fun + "," + this->dispatch(node.parent) + ", " + this->dispatch(node.child[0]) + ")";
    }

    return out;
}

std::string Transpiler::visit_ternary(TernaryNode& node) {
    std::string out;
    out += "((it=(" + this->dispatch(node.expression) + "))!=nullptr)? (" + this->dispatch(node.true_case) + ") : (" +
           this->dispatch(node.false_case) + ")";
    return out;
}

std::string Transpiler::visit_tuple(TupleNode& node) {
    std::string out;
    out += "TUPLE" + std::to_string(node.values.size()) + "(";
    for (int i = 0; i < node.values.size(); i++) {
        out += this->dispatch(node.values[i]) + ", ";
    }
    if (node.values.size() != 0) {
        out = out.substr(0, out.size() - 2);
    }
    out += ")";
    return out;
}

std::string Transpiler::visit_type(TypeNode& node) {
    return "";
}

std::string Transpiler::visit_while(WhileNode& node) {
    std::string out;
    out += "while(";
    out += "GET_BOOL(" + this->dispatch(node.condition) + ")";
    out += ")";
    out += "{";
    out += this->dispatch(node.body);
    out += "}";
    return out;
}

std::string Transpiler::dispatch(Node* nptr) {
    Node& n = *nptr;
    switch (n.ntype) {
        case NodeType::METHOD:
            return this->visit_method(n.method());
        case NodeType::CAST:
            return this->visit_cast_op(n.cast());
        case NodeType::BOOLOP:
            return this->visit_bool_op(n.boolop());
        case NodeType::ASSIGN:
            return this->visit_assignment(n.assign());
        case NodeType::BINOP:
            return this->visit_binop(n.binop());
        case NodeType::BLOCK:
            return this->visit_block(n.block());
        case NodeType::BOOLEAN:
            return this->visit_boolean(n.boolean());
        case NodeType::FUNC:
            return this->visit_function(n.func());
        case NodeType::BRK:
            return this->visit_break(n.brk());
        case NodeType::CALL:
            return this->visit_call(n.call());
        case NodeType::CLS:
            return this->visit_class(n.cls());
        case NodeType::CNTINUE:
            return this->visit_continue(n.cntinue());
        case NodeType::DECL:
            return this->visit_declaration(n.decl());
        case NodeType::EMPTYLST:
            return this->visit_empty_list(n.emptylst());
        case NodeType::FORLOOP:
            return this->visit_for(n.forloop());
        case NodeType::ID:
            return this->visit_id(n.id());
        case NodeType::IFF:
            return this->visit_if(n.iff());
        case NodeType::LST:
            return this->visit_list(n.lst());
        case NodeType::MEMBER:
            return this->visit_member(n.member());
        case NodeType::NONE:
            return this->visit_none(n.none());
        case NodeType::NUMBER:
            return this->visit_number(n.number());
        case NodeType::RETRN:
            return this->visit_return(n.retrn());
        case NodeType::STRNG:
            return this->visit_string(n.strng());
        case NodeType::SUB:
            return this->visit_subscript(n.sub());
        case NodeType::TERNARY:
            return this->visit_ternary(n.ternary());
        case NodeType::OTYPE:
            throw std::runtime_error("Don't know what to do!");
        case NodeType::FTYPE:
            throw std::runtime_error("Don't know what to do!");
        case NodeType::WHIL:
            return this->visit_while(n.whil());
        case TUPLE:
            return this->visit_tuple(n.tuple());
        case PARTIAL:
            return this->visit_partial(n.partial());
        case IMPORT:
            return "";
            break;
        case DICT:
            return this->visit_dict(n.dict());
            break;
        case EMPTYDICT:
            break;
        case NodeType::DEF_CONST:
            return this->visit_default_constructor(n.defconst());
        default:
            throw std::runtime_error("Don't know what to do!");
    }
}

void Transpiler::foo() {

}

std::string Transpiler::generate_tuple(int n) {
    std::string tuple_name = "Tuple" + std::to_string(n);
    std::string out = "class " + tuple_name + " : public Tuple";
    std::string type;
    std::string members;
    for (int i = 0; i < n; i++) {
        members += "XObject* mem_" + std::to_string(i + 1) + ";";
    }
    members = "XObject* members[" + std::to_string(n) + "];";
    out += type + "{public:\n" + members;
    std::string constructor = tuple_name + "(";
    for (int i = 0; i < n; i++) {
        constructor += "XObject* mem_" + std::to_string(i + 1) + ", ";
    }
    constructor = constructor.substr(0, constructor.size() - 2);
    constructor += "):Tuple(\"" + tuple_name + "\"){";
    for (int i = 0; i < n; i++) {
        constructor += "this->members[" + std::to_string(i) + "]= mem_" + std::to_string(i + 1) + ";\n";
    }
    constructor += "}\n";
    out += constructor;
    std::string get_member = "XObject* get_member(int i) override{";
    get_member += "assert(i>=1 && i<=" + std::to_string(n) + ");";
    get_member += "return this->members[i-1];";
    get_member += "}";
    out += get_member;
    out += "};";

    return out;
}


std::string Transpiler::generate_tuple_types() {
    std::string out;
    return out;
}

Transpiler::Transpiler(std::map<std::string, std::string>& map, std::string current_module, std::string includes) : map(
        map) {
    this->is_lvalue = false;
    this->is_call = false;
    this->current_module = current_module;

    this->m_header = "#ifndef " + this->current_module + "\n";
    this->m_header += "#define " + this->current_module + "\n";
    this->m_header += includes;
}


std::string generate_function_class(int num_args) {
    std::string out = "template <";
    for (int i = 0; i < num_args + 1; i++) {
        out += "typename " + std::string(1, 'A' + i) + ", ";
    };
    out = out.substr(0, out.size() - 2);
    out += ">\n";
    out += "class Function" + std::to_string(num_args) + "{\n";
    out += "public:\n";
    out += "virtual " + std::string(1, (char) ('A' + num_args)) + " call(";
    for (int i = 0; i < num_args; i++) {
        out += std::string(1, 'A' + i) + " " + std::string(1, 'a' + i) + ", ";
    };
    if (num_args != 0) {
        out = out.substr(0, out.size() - 2);
    }
    out += ") = 0;\n";
    out += "};";
    return out;
}

std::string Transpiler::visit_partial(PartialApplication& node) {
    std::string out;
    std::string args;
    std::vector<bool> provided_args(node.args.size(), false);
    int num_args = 0;
    for (int i = 0; i < node.args.size(); i++) {
        if (node.args[i] == nullptr) {
            num_args++;
            args += "nullptr, ";
        } else {
            args += this->dispatch(node.args[i]) + ", ";
        }
    }
    if (node.args.size() != 0) {
        args = args.substr(0, args.size() - 2);
    }
    out += "NEW(Partial" + std::to_string(num_args) + "," + this->dispatch(node.function) + "," + args + ")";
    return out;
}

std::string Transpiler::visit_cast_op(CastNode& node) {
    std::string name = node.exp_type->to_string() + "_to_" + node.as_type;
    std::string out = name + "(" + this->dispatch(node.exp) + ")";
    return out;
}

std::string Transpiler::visit_method(MethodNode& node) {
    std::string fn = this->map[node.actual_function_name];
    for (int i = 0; i < fn.size(); i++) {
        if (fn[i] == '.') {
            fn[i] = '_';
        }
    }
    std::string out =
            "NEW(Partial" + std::to_string(node.n_partial) + ", " + fn + ", " + this->dispatch(node.parent) + ", ";
    for (int i = 0; i < node.n_partial; i++) {
        out += "nullptr, ";
    }
    out = out.substr(0, out.size() - 2);
    out += ")";
    return out;
}

std::string Transpiler::visit_default_constructor(DefaultConstructorNode& node) {
    return this->map[node.name+".init"];
}

