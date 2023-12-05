# Sample Code

## Graph class
`graph.hpp` and `graph.cpp` contain a simple class to model unweighed undirected graphs.
You may modify and use them if and as you wish.
For convenience, the graph already supports input and output from and to the DIMACS format.

## Main routine
`example.cpp` contains a toy `main` routine that, for demonstration purposes, reads in a weighted graph in DIMACS
format, computes a mwpm using blossom5 and writes the result to stdout.

## Makefiles

In the Make.config file, you can change e.g. the c++ compiler which should be used (`CXX`). In order to to use the make
setup, open up a terminal and navigate to this folder. You can write `make debug` in order to create an executable in
which the compiler stayed close to your code and generated debug symbols which can e.g. be used in the gnu debugger gdb.
You can also write `make opt` in order to create an executable in which the compiler was allowed to do a lot of
optimization as long as the result stays the same. This executable is much faster, but not as usefull if you are still
testing if your program correctly, or why it does not. Both variants use all the error flags used in the problem
specification, meaning the compilation will fail if there are any warnings. This is as otherwise one might miss
important warnings, which can save a lot of time one would otherwise spend debugging! Finally you can write `make clean`
in order to remove everything generated when building one of the other make targets, including the executables and the
output directory.

You do not need to read the Makefile itself (neither did I, it's from a previous year), but you can if you want to. It
automatically compiles all files ending with `.cpp` using the C++ compiler specified as `CXX` in the Make.config and
compiles all files ending with `.c` using the C compiler specified as `CC` in the same file. Finally it links all your
objectfiles together with blossom5.a into a binary which is referenced by the symlink
`build/minimum_weight_perfect_matching`. Note exactly one of your `.cpp` and `.c` files should include a main function!
