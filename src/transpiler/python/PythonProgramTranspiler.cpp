//
// Created by chris on 11/10/21.
//

#include "PythonProgramTranspiler.h"
#include <util/utils.h>
#include <set>
#include <units/infos/Package.h>

void PythonProgramTranspiler::transpile_module(Module& module, const std::string& output_dir) {
    PythonModuleTranspiler transpiler(module);
    std::string module_code = transpiler.transpile_module(*module.sast, module.path);
    // std::cout << module_code << std::endl;
    const std::string& module_output_path = path_join(output_dir, module.name + ".py");
    std::ofstream out_module(module_output_path);
    std::set<std::string> imports;
    for (auto& import: module.imported_paths) {
        // auto v = import.second.as_vec();
        // if (v.size() > 1) {
        //     v.pop_back();
        // }
        // const std::string& imported_path = join(v, ".");
        // imports.insert(imported_path);
        // std::cout << "adding import: " << import.second.as_str() << " -> " << imported_path << std::endl;
        auto pv = import.second.as_vec();
        pv.pop_back();
        std::string pre = join(pv, ".");
        std::string name = import.first;
        out_module << "from " << pre << " import " << name << std::endl;
    }
    // for (auto& i: imports) {
    //     out_module << "import " << i << std::endl;
    // }
    out_module << module_code;

    for (auto& n: module.sast.get()->nodes) {
        if (n->type == sem::TopType::FUNCTION && static_cast<sem::FunctionDef&>(*n).identifier == "main") {
            out_module << "main()";
        }
    }
}

void PythonProgramTranspiler::transpile_package(Package& package, const std::string& output_dir) {
    const std::string& package_dir = path_join(output_dir, package.name);
    mkdir(package_dir.c_str(), 0700);
    std::ofstream init_file(path_join(package_dir, "__init__.py"));
    init_file.close();
    for (auto& u: package.units) {
        if (u.second->is_module()) {
            transpile_module(u.second->module(), package_dir);
        } else {
            transpile_package(u.second->package(), package_dir);
        }
    }
}
