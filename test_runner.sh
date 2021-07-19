RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color


cd build || exit
cmake .. > cmake_output
cmake_status=$?
if [ $cmake_status != 0 ]
then
  echo $cmake_status
  echo "[] Compiler cmake failed: "
  cat cmake_output
  exit 1
fi
make -j > compiler_make_output
compiler_make_status=$?
if [ $compiler_make_status != 0 ]
then
  echo $compiler_make_status
  echo "[] Compiler cmake failed: "
  cat compiler_make_output
  exit 1
fi
cd ..
export XLPATH=/home/chris/CLionProjects/compiler/lib
#set -x
top=$(pwd)
for TEST in full_tests/*.xl; do
  cd $top
  rm -rf temp
  mkdir temp
  mkdir temp/src
  echo "core==1.0.0" > temp/src/requirements.txt
  mkdir temp/out
  cp "$TEST" temp/src
  output_file=$(echo $TEST | sed 's/.xl/.out/g')
  echo -e "$BLUE" "---------- Testing $TEST" " ----------$NC"
  cp "$output_file" temp
  ./build/src/compiler/compiler -i temp/src -o temp/out -n test > compilation_output 2> compilation_error_output
  compilation_status=$?
  if [ $compilation_status != 0 ]
  then
    echo "[] Compilation failed: "
    cat compilation_output
    cat compilation_error_output
    continue
  fi
  mkdir temp/build
  cd temp/build
  cmake ../out > cmake_output
  cmake_status=$?
  if [ $cmake_status != 0 ]
  then
    echo "[] CMake failed"
    echo -e "$RED" "$(cat cmake_output)" "$NC"
    continue
  fi
  make -j > make_output
  make_status=$?
  if [ $make_status != 0 ]
  then
    echo "[] make failed"
    cat make_output
    continue
  fi
  cd ..
  ./build/test > program_output
  echo $(pwd)
  diff -q program_output "$(basename "$output_file")" > diff_output
  diff_status=$?
  if [ $diff_status != 0 ]
  then
    echo "Test for file " "$TEST" "$RED" " FAILED" "$NC"
    cat diff_output
  else
    echo -e "Test for file " "$TEST" "$GREEN" " PASSED" "$NC"
  fi
  echo $output_ok
#  break
  rm -rf temp
done

echo "FINISHED"