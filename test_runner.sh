cd build || exit
cmake ..
make -j
cd ..
export XLPATH=/home/chris/CLionProjects/compiler/lib
for TEST in full_tests/*.xl; do
  rm -rf temp
  mkdir temp
  mkdir temp/src
  echo "core==1.0.0" > temp/src/requirements.txt
  mkdir temp/out
  cp "$TEST" temp/src
  ./build/src/compiler/compiler -i temp/src -o temp/out -n test
  w=$?
  echo "PRE--------------"
  echo "return value: $w"
  echo "DONE--------------"
  break
#  rm -rf temp
done