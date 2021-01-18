//
// Created by chris on 6/10/20.
//

#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <scanner/Scanner.h>
#include <parser/Parser.h>
#include <formatter/Formatter.h>
#include <nodes/BlockNode.h>
#include <run.h>
#include <dirent.h>


void open_and_compile(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        exit(1);
    }
    std::stringstream sstream;
    sstream << file.rdbuf();
    std::string text = sstream.str();
    compile_and_run(text);
}

VectorOfStrings get_all_files_in_directory(std::string dirpath) {
    DIR* dir;
    struct dirent* ent;
    VectorOfStrings files;
    if ((dir = opendir(dirpath.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            if (std::string(ent->d_name) == "." || std::string(ent->d_name) == "..") continue;
            files.push_back(ent->d_name);
        }
        closedir(dir);
    } else {
        /* could not open directory */
        perror("");
    }
    return files;
}

void test_file(std::string filename) {
    std::cout << "*** TESTING FILE " << filename << " ***" << std::endl;
    open_and_compile(filename);
}

void should_fail_test(std::string f) {
    try {
        test_file("should_fail/" + f);
        FAIL() << "Should have failed but it did not!";
    } catch (const std::runtime_error& e) {
        std::cout << "AS EXPECTED: it threw \n " << e.what() << std::endl;
    }
}

TEST(run_test, call_simple_generic_function) {
    test_file("should_pass/"  "call_simple_generic_function.pl");
}

TEST(run_test, normal_function) {
    test_file("should_pass/"  "normal_function.pl");
}

TEST(run_test, simple_generic_function) {
    test_file("should_pass/"  "simple_generic_function.pl");
}

TEST(run_test, test_file) {
    test_file("should_pass/"  "test_file.xl");
}

TEST(run_test, call_complex_generic_function) {
    test_file("should_pass/"  "call_complex_generic_function.pl");
}

TEST(run_test, custom_class_simple) {
    test_file("should_pass/"  "custom_class_simple.pl");
}

TEST(run_test, class_instantiate) {
    test_file("should_pass/"  "class_instantiate.pl");
}

TEST(run_test, custom_class_with_method) {
    test_file("should_pass/"  "custom_class_with_method.pl");
}

TEST(run_test, call_function_ok) {
    test_file("should_pass/"  "call_function_ok.pl");
}


TEST(run_test, simple_list) {
    test_file("should_pass/"  "simple_list.pl");
}

TEST(run_test, get_list_subscript) {
    test_file("should_pass/"  "get_list_subscript.pl");
}

TEST(run_test, call_method) {
    test_file("should_pass/"  "call_method.pl");
}


TEST(run_test, call_method_with_args) {
    test_file("should_pass/"  "call_method_with_args.pl");
}

TEST(run_test, call_non_existent_function) {
    should_fail_test("call_non_existent_function.pl");
}

TEST(run_test, call_simple_function_wrong_argument_types) {
    should_fail_test("call_simple_function_wrong_argument_types.pl");
}

TEST(run_test, call_simple_function_wrong_number_of_arguments) {
    should_fail_test("call_simple_function_wrong_number_of_arguments.pl");
}

TEST(run_test, call_complex_generic_function_error) {
    should_fail_test("call_complex_generic_function_error.pl");
}

TEST(run_test, class_no_member_found_error) {
    should_fail_test("class_no_member_found_error.pl");
}


TEST(run_test, instantiate_non_existant_class_error) {
    should_fail_test("instantiate_non_existant_class_error.pl");
}

TEST(run_test, instantiate_class_bad_member_error) {
    should_fail_test("instantiate_class_bad_member_error.pl");
}

TEST(run_test, instantiate_class_bad_member_type_error) {
    should_fail_test("instantiate_class_bad_member_type_error.pl");
}

TEST(run_test, call_function_wrong_arg_type_error) {
    should_fail_test("call_function_wrong_arg_type_error.pl");
}

TEST(run_test, cant_overload_function_error) {
    should_fail_test("cant_overload_function_error.pl");
}

TEST(run_test, call_method_with_args_error) {
    should_fail_test("call_method_with_args_error.pl");
}