# Sample Code

## Graph class
`graph.hpp` and `graph.cpp` contain a simple class to model unweighed undirected graphs that you may use if you wish.
For convenience, the graph already supports input and output from and to the DIMACS format.

## Main routine
`example.cpp` contains a toy `main` routine that, for demonstration purposes,
reads in a graph in DIMACS format, greedily removes edges until
every node is incident to at most one edge and outputs the result to stdout.

## Makefiles
This code also contains a Makefile that you can use for compilation if you prefer it to a manual compilation command (but you don't have to).
In the Make.config file, you can change e.g. the c++ compiler which should be used (`CXX`).
In order to to use the make setup, open up a terminal and navigate to this folder.
You can write `make debug` in order to create an executable in which the compiler stayed close
to your code and generated debug symbols which can e.g. be used in the gnu debugger gdb.
You can also write `make opt` in order to create an executable in which the compiler was
allowed to do a lot of optimization as long as the result stays the same.
This executable is much faster, but not as usefull if you are still testing
if your program correctly, or why it does not.
Both variants use all the error flags used in the problem specification,
meaning the compilation will fail if there are any warnings.
This is as otherwise one might miss important warnings,
which can save a lot of time one would otherwise spend debugging!
Finally you can write `make clean` in order to remove everything generated when building
one of the other make targets, including the executables and the output directory.

You do not need to read the Makefile itself, but you can if you want to.
It automatically compiles all files ending with `.cpp`, `.C` and `.CPP` using the C++ compiler specified as `CXX` in the Make.config,
and compiles all files ending with `.c` using the C compiler specified as `CC` in the same file.
Finally it links everything together into a binary which is referenced by the symlink `build/main`.
Note exactly one of your `.C` and `.c` files should include a main function!

