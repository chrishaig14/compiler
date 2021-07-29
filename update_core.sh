cd ~/CLionProjects/compiler/lib/core/1.0.0/build
cmake ../out
make -j
cd ~/CLionProjects/compiler/lib/build/
rm libcore*
ln ~/CLionProjects/compiler/lib/core/1.0.0/build/libcore-1.0.0.a