## Graph class
`graph.hpp` and `graph.cpp` contain a simple class to model unweighed undirected graphs.
`edmonds.cpp` contains definitions of functions concerning computing maximum
matching. These functions are declared in `graph.hpp`.

## Main routine
`main.cpp` contains a `main` routine that reads file, which is given as an
argument, and calculates a maximum matching. It then outputs the matching to
standard output.

## Makefiles
There is included Makefile, which is almost the same as the one provided to us.
What is changed is that now it only compiles `main.cpp` (compiling all `.cpp`
files resulted in an error concerning some multiple definitions). We also
changed default compilation mode to "opt", instead of "debug", because it
produces much faster code.
