#include <set>
#include "utils.h"
#include "Compiler.h"
#include "../transpiler/PythonTranspiler.h"

std::string join(VectorOfStrings v, std::string s) {
    std::string result;
    for (auto& x: v) {
        result += x + s;
    }
    result = result.substr(0, result.size() - s.size());
    return result;
}

void transpile_module(Module& module, const std::string& output_dir) {
    PythonTranspiler transpiler;
    std::string module_code = transpiler.transpile_module(*module.sast, module.path);
    // std::cout << module_code << std::endl;
    const std::string& module_output_path = path_join(output_dir, module.name + ".py");
    std::ofstream out_module(module_output_path);
    std::set<std::string> imports;
    for (auto& import: module.imported_paths_no_alias) {
        auto v = import.second.as_vec();
        if (v.size() > 1) {
            v.pop_back();
        }
        const std::string& imported_path = join(v, ".");
        imports.insert(imported_path);
        std::cout << "adding import: " << import.second.as_str() << " -> " << imported_path << std::endl;
    }
    for (auto& i: imports) {
        out_module << "import " << i << std::endl;
    }
    out_module << module_code;

    for (auto& n: module.sast.get()->nodes) {
        if (n->type == sem::TopType::FUNCTION && static_cast<sem::FunctionDef&>(*n).identifier == "main") {
            out_module << "main()";

        }
    }
}

void transpile_package(Package& package, const std::string& output_dir) {
    const std::string& package_dir = path_join(output_dir, package.name);
    mkdir(package_dir.c_str(), 0700);
    std::ofstream init_file(path_join(package_dir, "__init__.py"));
    init_file.close();
    for (auto u: package.units) {
        if (u.second->is_module()) {
            transpile_module(u.second->module(), package_dir);
        } else {
            transpile_package(u.second->package(), package_dir);
        }
    }
}

int main(int argc, char* argv[]) {
    bool is_lib;
    std::string project_dir;
    std::string project_output_dir;
    std::string output_name;
    char* xlpath = getenv("XLPATH");
    assert(xlpath != nullptr);
    std::string lib_path = xlpath;
    std::string version;
    // std::cout << "argc = " << argc << std::endl;
    // for (size_t i = 0; i < (size_t) argc; i++) {
    //     std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    // }
    init(argc, argv, is_lib, project_dir, project_output_dir, output_name, version);

    Compiler compiler(project_dir, project_output_dir, output_name, lib_path, is_lib, version);
    compiler.pre();
    if (not compiler.main()) {
        exit(1);
    }
    transpile_package(compiler.root_package, project_output_dir);
}