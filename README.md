# Minimum Vertex Cover Solver

Simple program built in C++ that incorporates a MiniSat SAT solver cloned from https://github.com/agurfinkel/minisat. The MiniSAT solver is implemented as described at this link (http://minisat.se/downloads/MiniSat.pdf). The implemented program does the following:
* Takes an undirected graph as an input
* Calculates the minimum vertex cover of the inputted graph

## CMake

The CMake instructions are contained in `CMakeLists.txt`. To compile the program, do the following, starting in the main repo:
```
mkdir build
cd build
cmake ../
make install
```

## Sample Run

Assume that the executable is called `ece650-a4`. In the following, `$` is the command-prompt.

```
$ ./ece650-a4
V 5
E {<0,4>,<4,1>,<0,3>,<3,4>,<3,2>,<1,3>}
3 4
```

In the above sample run, the lines "V = ...", "E = ...." are inputs and "3 4" is the output.


## Encoding to SAT of Vertex Cover Problem

For information about the encoding used as part of this program, go to `encoding.pdf` file in the main repository of this project
