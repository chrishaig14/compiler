//
// Created by chris on 5/9/21.
//

#include "Module.h"
#include "Import.h"
#include "TemplateClassDef.h"
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

ast::Module::Module(std::vector<std::unique_ptr<ast::TopNode>> all,
                    std::vector<std::reference_wrapper<ast::Import>> imports,
                    std::vector<std::reference_wrapper<ast::ConcreteClassDef>> classes,
                    std::vector<std::reference_wrapper<ast::TemplateClassDef>> template_classes,
                    std::vector<std::reference_wrapper<ast::EnumNode>> enums,
                    std::vector<std::reference_wrapper<ast::Function>> functions,
                    std::vector<std::reference_wrapper<ast::TypeclassAst>> typeclasses,
                    std::vector<std::reference_wrapper<ast::Instance>> instances)
        : imports(imports), classes(classes), template_classes(template_classes), enums(enums), functions(functions),
          typeclasses(typeclasses), instances(instances),all(std::move(all)) {
}

