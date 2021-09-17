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
    compiler.main();
    PythonTranspiler transpiler;
    Module& module = *compiler.root_package.units.at("main").module;
    std::string module_code = transpiler.transpile_module(*module.sast);
    std::cout << module_code << std::endl;
    std::ofstream out_module(path_join(project_output_dir, "out.py"));
    for (auto& import: module.imported_paths_no_alias) {
        std::cout << "adding import: " << import.second.as_str();
        auto v = import.second.as_vec();
        v.pop_back();
        out_module << "import " << join(v, ".") << std::endl;
    }
    out_module << module_code;
    out_module << "main()";
}