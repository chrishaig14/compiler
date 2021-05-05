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


def test(test_file):
    print(f"testing: {test_file}")
    with tempfile.TemporaryDirectory() as topdirname:
        topdirname = "/home/chris/CLionProjects/compiler/random"
        shutil.rmtree(topdirname, ignore_errors=True)
        os.mkdir(topdirname)
        dirname = topdirname + "/" + test_file[:-3]
        os.mkdir(dirname)
        test_f = open(os.path.join(TEST_FILE_DIR, test_file))

        out_file = open(os.path.join(dirname, test_file), "w")
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
        out_file.writelines(lines)
        out_file.flush()
        test_f.flush()

        test_f = open(os.path.join(TEST_FILE_DIR, test_file))

        out_file = open(os.path.join(dirname, test_file), "r")

        ts = test_f.read()
        # print(ts)
        ots = out_file.read()
        # print(ots)

        out_dirname = topdirname + "/output"
        os.mkdir(out_dirname)

        copy_file(os.path.join(TEST_FILE_DIR, CORE_FILE), os.path.join(dirname, CORE_FILE))
        s = time.time()
        result = subprocess.run([os.path.join(os.path.dirname(__file__), "build/compiler"), dirname, out_dirname],
                                capture_output=True)
        e = time.time()
        print(f"FIRST COMPILATION TIME: {e - s}")
        # result = subprocess.call([os.path.join(os.path.dirname(__file__), "build/compiler"), dirname, "bar"])
        if result.returncode != 0:
            print_formatted_text(HTML(f"test for {test_file} <b><red>FAILED</red></b>"))
            print(result.stdout.decode())
            print(result.stderr.decode())
            return
        # print(result.stdout.decode())

        # compilation ok, now compile generated c++ code

        out_dir = os.scandir(out_dirname)
        # print("Output dir:")
        # for x in out_dir:
        #     print(x.path)

        build_dir = out_dirname + "/build"
        os.mkdir(build_dir)
        s = time.time()
        # print("RUNNING CMAKE .. in BUILD DIR : ", build_dir)
        result = subprocess.run(["cmake", ".."], cwd=build_dir,
                                capture_output=True)
        e = time.time()
        # exit(0)
        # print(f"CMAKE TIME: {e - s}")
        if result.returncode != 0:
            print_formatted_text(HTML(f"test for {test_file} <b><red>FAILED</red></b>"))
            print(result.stdout.decode())
            print(result.stderr.decode())
            return

        # make
        s = time.time()
        result = subprocess.run(["make"], cwd=build_dir,
                                capture_output=True)
        e = time.time()
        # print(result.stdout.decode())
        # print(f"MAKE TIME: {e - s}")
        # run!

        s = time.time()
        result = subprocess.run(["./result"], cwd=build_dir + "/application",
                                capture_output=True)
        e = time.time()
        # print(f"RUNTIME: {e - s}")
        # print(result.stdout.decode())

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
