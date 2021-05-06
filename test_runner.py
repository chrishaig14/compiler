import os
import shutil
import subprocess
import tempfile
import time

from prompt_toolkit import print_formatted_text, HTML

TEST_FILE_DIR = "full_tests"
CORE_FILE = "core.xl"


def copy_file(src, dest):
    dest_file = open(dest, "w")
    dest_file.write(open(src).read())


def run_cmd(cmd, dir):
    s = time.time()
    result = subprocess.run(cmd, cwd=dir, capture_output=True)
    e = time.time()
    print(f"{e - s}")
    if result.returncode != 0:
        print(f'"{" ".join(cmd)}" failed with: {result.returncode} in {e - s}s')
        print()
        print("-- STDOUT --")
        print()
        print(result.stdout.decode())
        print()
        print("-- STDERR --")
        print()
        print(result.stderr.decode())
        print()
        exit(1)
    print(f'"{" ".join(cmd)}" finished succesfully in {e - s}s')
    return result


def parse_test_file(test_f):
    expected_output = []
    in_output = False
    lines = []
    end_exp_output = False
    for l in test_f.readlines():
        if l.strip() == "#####":
            if in_output:
                end_exp_output = True
            in_output = True
            continue
        if end_exp_output:
            continue
        if in_output:
            expected_output += [l]
        else:
            lines += [l]
    return lines, expected_output


def test(test_file):
    print(f"testing: {test_file}")
    with tempfile.TemporaryDirectory() as topdirname:
        root_dir = os.path.dirname(__file__)
        topdirname = root_dir + "/random"
        shutil.rmtree(topdirname, ignore_errors=True)
        os.mkdir(topdirname)
        dirname = topdirname + "/" + test_file[:-3]
        os.mkdir(dirname)
        test_f = open(os.path.join(TEST_FILE_DIR, test_file))

        out_file = open(os.path.join(dirname, test_file), "w")
        source, expected_output = parse_test_file(test_f)

        out_file.writelines(source)
        out_file.flush()
        test_f.flush()

        out_dirname = topdirname + "/output"
        os.mkdir(out_dirname)

        copy_file(os.path.join(TEST_FILE_DIR, CORE_FILE), os.path.join(dirname, CORE_FILE))

        compile_cmd = ["build/compiler", dirname, out_dirname]
        run_cmd(compile_cmd, root_dir)

        # compilation ok, now compile generated c++ code

        build_dir = out_dirname + "/build"
        os.mkdir(build_dir)

        cmake_result_cmd = ["cmake", ".."]
        run_cmd(cmake_result_cmd, build_dir)

        # make
        make_result_cmd = ["make", "-j"]
        run_cmd(make_result_cmd, build_dir)

        # run program
        make_result_cmd = ["./result"]
        result = run_cmd(make_result_cmd, build_dir + "/application")

        exp_out = "".join(expected_output)
        outp = result.stdout.decode()
        s = os.get_terminal_size()
        hr = HTML("<b>" + s.columns * "-" + "</b>")

        if outp != exp_out:
            print_formatted_text(HTML(f"test for {test_file} <b><red>FAILED</red></b>"))
            print()
            print_formatted_text(HTML(f"<red><b>Expected stdout</b></red>"))
            print_formatted_text(hr)
            print(exp_out)
            print_formatted_text(hr)
            print()
            print_formatted_text(HTML(f"<green><b>Actual stdout</b></green>"))
            print_formatted_text(hr)
            print(outp)
            print_formatted_text(hr)


def main():
    test_files = os.scandir(TEST_FILE_DIR)
    start = time.time()

    compiler_build_dir = "/home/chris/CLionProjects/compiler/build"

    cmake_compiler_cmd = ["cmake", ".."]
    run_cmd(cmake_compiler_cmd, compiler_build_dir)

    make_clean_cmd = ["make", "clean"]
    run_cmd(make_clean_cmd, compiler_build_dir)

    make_compiler_cmd = ["make", "compiler", "-j"]
    run_cmd(make_compiler_cmd, compiler_build_dir)

    for tf in test_files:
        if tf.name == CORE_FILE:
            continue
        s = time.time()
        test(tf.name)
        e = time.time()
        print(f"PARTIAL TIME: {e - s}")
    end = time.time()
    print(f"TOTAL TIME: {end - start}")


if __name__ == '__main__':
    main()
