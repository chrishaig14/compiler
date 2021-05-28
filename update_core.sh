cd /home/chris/CLionProjects/compiler/lib/core/1.0.0/build
cmake ../out
make -j
cd /home/chris/CLionProjects/compiler/lib/build/
rm libcore*
ln /home/chris/CLionProjects/compiler/lib/core/1.0.0/build/libcore-1.0.0.a