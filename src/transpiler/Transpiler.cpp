//
// Created by chris on 2/8/20.
//

#include <iostream>
#include "Transpiler.h"
#include "../macros.h"
#include "../semantic/Checker.h"

#define FUNCTION_PREFIX "function_"

std::string Transpiler::transpile(BlockNode* node) {
    std::string out = this->generate_tuple_types();
    out += this->dispatch(node);
    return out;
}

inline std::string get_class_name(const std::string& c) {
    return "class_" + c;
}

std::string Transpiler::visit_assignment(AssignmentNode& node) {
    std::string out;
    this->is_lvalue = true;
    if (node.lvalue->ntype == SUB) {
        out += "set_subscript(" + this->dispatch(node.lvalue->sub().parent) + ", " +
               this->dispatch(node.lvalue->sub().child[0]) + ", " +
               this->dispatch(node.rvalue) + ")";
    } else if (node.lvalue->ntype == MEMBER) {
        this->is_lvalue = false;
        MemberNode& memberNode = node.lvalue->member();
        TypeNode* cast_type = memberNode.parent_t;
        out += "CAST(" + this->dispatch(node.lvalue->member().parent) + "," +
               get_class_name(cast_type->to_string()) + ")->";
        out += node.lvalue->member().s_child + "=";
        out += this->dispatch(node.rvalue) + "";
    } else {
        out += this->dispatch(node.lvalue) + " = ";
        this->is_lvalue = false;
        std::string var_type = this->type_mapper(*node.type);
        out += this->dispatch(node.rvalue) + ";";
        out += "SET(" + this->dispatch(node.lvalue) + ")";
    }
    return out;
}

std::string Transpiler::visit_binop(BinopNode& node) {
    std::string out = "op_";
    switch (node.op) {
        case OpType::ADD:
            out = "INT_ADD";
            break;
        case OpType::SUB:
            out = "INT_SUB";
            break;
        case OpType::MUL:
            out = "INT_MUL";
            break;
        case OpType::DIV:
            out = "INT_DIV";
            break;
        case OpType::MOD:
            out = "INT_MOD";
            break;
    }
    out += "(" + this->dispatch(node.left) + ", " + this->dispatch(node.right) + ")";
    return out;
}

std::string Transpiler::visit_block(BlockNode& node) {
    std::string out;
    for (auto n: node.nodes) {
        out += this->dispatch(n);
        if (n->ntype != FUNC && n->ntype != WHIL && n->ntype != IFF && n->ntype != CLS) {
            out += ";";
        }
    }
    return out;
}

std::string Transpiler::visit_boolean(BooleanNode& node) {
    return node.value ? "BOOL_TO_PTR(true)" : "BOOL_TO_PTR(false)";
}

std::string Transpiler::visit_bool_op(BoolOpNode& node) {
    std::string out = "op";
    switch (node.op) {
        case BoolOp::EQ:
            out += "_eq";
            break;
        case BoolOp::AND:
            out += "_and";
            break;
        case BoolOp::OR:
            out += "_or";
            break;
        case BoolOp::LEQ:
            out += "_leq";
            break;
        case BoolOp::GEQ:
            out += "_geq";
            break;
        case BoolOp::LT:
            out += "_lt";
            break;
        case BoolOp::GT:
            out += "_gt";
            break;
        case BoolOp::NEQ:
            out += "_neq";
            break;
    }
    out += "(";
    out += this->dispatch(node.left) + ", " + this->dispatch(node.right) + ")";
    return out;
}

std::string Transpiler::visit_break(BreakNode& node) { return ""; }

std::string Transpiler::visit_call(CallNode& node) {
    std::string out;
    out += "CALL" + std::to_string(node.arguments.size()) + "(";
    out += this->visit_id(node.function->id()) + ",";
    for (int i = 0; i < node.arguments.size(); i++) {
        std::string w = this->dispatch(node.arguments[i]);
        if (node.arguments[i]->ntype != ID) {
            w = "TEMP(" + w + ")";
        }
        out += w + ", ";
    }
    if (node.arguments.size() != 0) {
        out = out.substr(0, out.size() - 2);
    }
    out += ")";
    return out;
}

std::string Transpiler::visit_class_literal_expression(ClassLiteralExpressionNode& node) {
    throw std::runtime_error("CLASS LITERAL EXPRESSION IN TRANSPILER!");
}

std::string Transpiler::visit_class_literal_field(ClassLiteralFieldNode& node) {
    std::string out;
    out = "GC::register_object(TAG(";
    out += "new " + get_class_name(node.type->id) + "(";
    for (int i = 0; i < node.init_names.size(); i++) {
        out += this->dispatch(node.init_values[i]) + ", ";
    }
    if (node.init_names.size() != 0) {
        out = out.substr(0, out.size() - 2);
    }
    out += ")";
    out += ")";
    out += ")";
    return out;
}

std::string Transpiler::visit_class(ClassNode& node) {
    std::string out;
    this->method_class = node.class_name;
    this->num_members_class = node.members.size();
    std::string class_name = get_class_name(node.class_name);
    out = "class " + class_name + " : public XUserObject {\n";
    std::sort(node.members_ordered.begin(), node.members_ordered.end());
    out += "public:\n";
    for (int i = 0; i < node.members_ordered.size(); i++) {
        out += "XObject* " + node.members_ordered[i] + ";\n";
    }
    out += "\n";
    out += class_name + "(";
    for (int i = 0; i < node.members_ordered.size(); i++) {
        out += "XObject* " + node.members_ordered[i] + ", ";
    }
    out = out.substr(0, out.size() - 2);
    out += "):";
    out += "XUserObject(\"" + class_name + "\")";
    out += "{\n";
    for (int i = 0; i < node.members_ordered.size(); i++) {
        out += "this->" + node.members_ordered[i] + " = " + node.members_ordered[i] + ";\n";
    }
    out += "}\n";
    std::string mark;
    mark += "void mark(std::vector<XObject*>& new_root)override{";
    // void mark(std::vector<XObject*>& new_root) override {
    //         for (auto& m: this->get_members()) {
    //             if (has_tag(m, OBJECT_TAG)) {
    //                 XObject* element = UNTAG(m);
    //                 if (!element->is_reachable() && !element->inserted) {
    //                     new_root.push_back(element);
    //                     element->inserted = true;
    //                 }
    //             }
    //         }
    // }
    for (auto m: node.members_ordered) {
        mark += "if (has_tag(this->" + m + ", OBJECT_TAG)){XObject* element = UNTAG(this->" + m +
                ");               if (!element->is_reachable() && !element->inserted) {\n"
                "                      new_root.push_back(element);\n"
                "                      element->inserted = true;\n"
                "                  }}";
    }
    mark += "}";
    out += mark;
    out += "};";
    for (auto n: node.methods) {
        std::string method_name = n.second->identifier;
        n.second->identifier = node.class_name + "_" + n.second->identifier;
        if (method_name != "init") {
            n.second->parameter_names.insert(n.second->parameter_names.begin(), "this_obj");
            n.second->parameter_types.insert(
                    n.second->parameter_types.begin(),
                    new ObjectType(node.class_name, {}));
        }
        out += this->dispatch(n.second);
    }
    return out;
}

std::string Transpiler::ptr_to_type_object(const ObjectType& t) {
    if (t.id.size() == 1 && islower(t.id[0])) {
        // it's a generic, return same without pointer;
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
                return "Tuple" + std::to_string(i) + "*";
            }
        }
    }
    if (t.id == "Option") {
        return this->type_mapper(*t.type_params[0]);
    }
    if (t.id == "List") {
        return "PTR_TO_LIST";
    }
    return get_class_name(t.id) + "*";
}


std::string Transpiler::object_type_mapper(const ObjectType& t) {
    if (t.id.size() == 1 && islower(t.id[0])) {
        // it's a generic, return same without pointer;
        return "XObject*";
    }
    if (t == T_INT) {
        return "XObject*";
    }
    if (t == T_STRING) {
        return "XObject*";
    }
    if (t == T_BOOL) {
        return "XObject*";
    }
    if (t.id == "Tuple") {
        for (int i = 0; i < this->tuple_types.size(); i++) {
            if (t == *this->tuple_types[i]) {
                return "Tuple" + std::to_string(i) + "*";
            }
        }
    }
    if (t.id == "Option") {
        return this->type_mapper(*t.type_params[0]);
    }
    if (t.id == "List") {
        return "XList*";
    }
    return get_class_name(t.id) + "*";
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
//    if (t.parameter_types.size() != 0) {
//        out = out.substr(0, out.size() - 2);
//    }
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

std::string Transpiler::visit_continue(ContinueNode& node) { return ""; }

std::string Transpiler::visit_declaration(DeclarationNode& node) {
    std::string out;
    for (int i = 0; i < node.identifier.size(); i++) {
        if (node.identifier[i] == '.') {
            node.identifier[i] = '_';
        }
    }
    // out += this->add_type(*node.type, node.identifier) + " = " + this->dispatch(node.expression);
    if (node.expression->ntype == TERNARY) {
        out += "it = " + this->dispatch(node.expression->ternary().expression) + ";\n";
    }
    std::string var_type = this->type_mapper(*node.type);
    // out += var_type + " " + node.identifier + " = " + "(" + this->ptr_to_type_object(node.type->object()) + ")(" +
    //        this->dispatch(node.expression) + ")";
    out += "XObject* " + node.identifier + " = " + this->dispatch(node.expression) + ";";
    out += "DECLARE(" + node.identifier + ")";
    return out;
}

std::string Transpiler::visit_empty_list(EmptyListNode& node) {
    return "NEW(XList,std::vector<XObject*>())";
}

std::string Transpiler::visit_float(FloatNode& node) { return ""; }

std::string Transpiler::visit_for(ForNode& node) {
    std::string out;
    out += "for(int myindex=0;myindex < ((XList*)(UNTAG(" + this->dispatch(node.exp) + ")))->l.size(); myindex++){\n";
    out += "ENTER();";
    out += "XObject* " + node.var + " = ((XList*)(UNTAG(" + this->dispatch(node.exp) + ")))->l[myindex];";
    out += "DECLARE(" + node.var + ");";
    out += this->dispatch(node.body);
    out += "LEAVE();";
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
    std::string out;
    // out += this->type_mapper(*node.return_type);
    std::string raw_function_name = "f_" + node.identifier;

    std::string signature;

    signature += "XObject*";
    signature += " ";
    signature += raw_function_name;
    signature += "(";
    for (int i = 0; i < node.parameter_types.size(); i++) {
        signature += std::string() + "XObject*" + " ptr_" + node.parameter_names[i] + ", ";
    }
    if (node.parameter_types.size() > 0) {
        signature = signature.substr(0, signature.size() - 2);
    }
    signature += ")";
    out += signature;
    this->header += signature + ";\n";
    out += "{\n";
    for (int i = 0; i < node.parameter_types.size(); i++) {
        out += this->type_mapper(*node.parameter_types[i]) + " " + node.parameter_names[i] + " = (" +
               this->type_mapper(*node.parameter_types[i]) + ")" + " ptr_" + node.parameter_names[i] + ";";
    }

    out += "void* it = nullptr;\n";
    out += "ENTER_FUN(" + node.identifier + ");";
    bool is_init = false;
    if (node.identifier == this->method_class + "_init") {
        is_init = true;
        out += "XObject* this_obj = NEW(";
        out += get_class_name(this->method_class) + ",";
        for (int i = 0; i < this->num_members_class; i++) {
            out += "nullptr, ";
        }
        if (this->num_members_class != 0) {
            out = out.substr(0, out.size() - 2);
        }
        out += ");\n";

    }
    out += this->dispatch(node.body);
    if (is_init) {
        out += "RETURN(this_obj);";
    } else if (node.return_type->kind == Kind::OBJECT && node.return_type->object().id == ".None") {
        out += "RETURN(nullptr);";
    }
    out += "}";
    std::string num_args_str = std::to_string(node.parameter_names.size());
    std::string function_obj_name = FUNCTION_PREFIX + node.identifier;

    std::string function_class = "Function" + num_args_str;
    this->externs_declaration += "extern  " + function_class + "* " + function_obj_name + "\n;";
    // this->static_declarations += "static Function" + num_args_str + "* " + function_obj_name + ";\n";
    this->globals_initialization += function_class + " " + raw_function_name + "_f" + " = " + function_class + " (" +
                                    raw_function_name + ");\n";
    this->globals_initialization += function_class + "* " + function_obj_name + "=&" + raw_function_name + "_f;";
    // out += "static Function" + num_args_str + "* " + function_obj_name + " = new Function" + num_args_str + "(" +
    //        raw_function_name + ");\n";
    return out;
}

std::string Transpiler::visit_id(IdNode& node) {
    std::string idn = node._id;
    if (node.is_global_function) {
        std::string out = node._id;
        for (int i = 0; i < out.size(); i++) {
            if (out[i] == '.') {
                out[i] = '_';
            }
        }
        return FUNCTION_PREFIX + out;
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
    out += "(PTR_TO_BOOL(" + this->dispatch(node.condition) + "))" + "{";
    out += "ENTER();";
    out += this->visit_block(*node.then);
    out += "LEAVE();";
    out += "}";
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
            w = "TEMP(" + w + ")";
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
    out = "NEW(XDict,std::unordered_map<XObject*,XObject*>({";
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
        return "((Tuple*)(UNTAG(" + this->dispatch(node.parent) + ")))->get_member(" + std::to_string(node.n_child) +
               ")";
    }
    return "CAST(" + this->dispatch(node.parent) + "," + get_class_name(node.parent_t->to_string()) + ")->" +
           node.s_child;
}

std::string Transpiler::visit_none(NoneNode& node) { return "nullptr"; }

std::string Transpiler::visit_number(NumberNode& node) {
    return std::string() + "INT_TO_PTR" + "(" + std::to_string(node.num) + ")";
}

std::string Transpiler::visit_return(ReturnNode& node) {
    std::string out;
    out += "RETURN(";
    if (node.expression != nullptr) {
        out += " " + this->dispatch(node.expression);
    } else {
        out += "nullptr";
    }
    out += ")";
    return out;

}

std::string Transpiler::visit_string(StringNode& node) {
    std::string out;
    out += "NEW(XString,\"" + node.str + "\")";
    return out;
}

std::string Transpiler::visit_subscript(SubscriptNode& node) {
    std::string out;
    std::string fun;
    if (node.parent_t->object().id == "List") {
        out = "LIST_SUBSCRIPT(" + this->dispatch(node.parent) + ", " + this->dispatch(node.child[0]) + ")";
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

std::string Transpiler::visit_type(TypeNode& node) { return ""; }

std::string Transpiler::visit_while(WhileNode& node) {
    std::string out;
    out += "while(";
    out += "PTR_TO_BOOL(" + this->dispatch(node.condition) + ")";
    out += ")";
    out += "{";
    out += "ENTER();";
    out += this->dispatch(node.body);
    out += "LEAVE();";
    out += "}";
    return out;
}

std::string Transpiler::dispatch(Node* nptr) {
    Node& n = *nptr;
    switch (n.ntype) {
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
        case NodeType::BRK:
            return this->visit_break(n.brk());
        case NodeType::CALL:
            return this->visit_call(n.call());
        case NodeType::CLSEXP:
            return this->visit_class_literal_expression(n.clsexp());
        case NodeType::CLSFLD:
            return this->visit_class_literal_field(n.clsfld());
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
        case NodeType::FUNC:
            return this->visit_function(n.func());
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
        case NodeType::FLOT:
            return this->visit_float(n.flot());
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
        case NodeType::UNINITIALIZED:
            throw std::runtime_error("Don't know what to do!");
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
    // for (int i = 2; i <= 16; i++) {
    //     out += this->generate_tuple(i);
    // }
    return out;
}

Transpiler::Transpiler() { this->is_lvalue = false; }


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

