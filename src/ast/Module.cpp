//
// Created by chris on 5/9/21.
//

#include "Module.h"
#include "Import.h"
#include "Klass.h"
#include "EnumNode.h"
#include "Function.h"

template<typename T>
std::vector<std::reference_wrapper<T>>
vector_of_unique_ptr_to_vector_of_reference_wrapper(const std::vector<std::unique_ptr<T>>& v) {
    std::vector<std::reference_wrapper<T>> r;
    for (auto& p: v) {
        r.push_back(*p);
    }
    return r;
}

ast::Module::Module(std::vector<std::unique_ptr<ast::Node>> all,
                    std::vector<std::reference_wrapper<ast::Import>> imports,
                    std::vector<std::reference_wrapper<ast::Klass>> classes,
                    std::vector<std::reference_wrapper<ast::EnumNode>> enums,
                    std::vector<std::reference_wrapper<ast::Function>> functions)
        : imports(imports), classes(classes), enums(enums), functions(functions), all(std::move(all)) {
}

