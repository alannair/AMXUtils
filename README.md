# AMXUtils
Matrix Multiplication Utilities for Intel's Advanced Matrix Extensions

## How to Install
First install sde:
https://software.intel.com/content/www/us/en/develop/articles/intel-software-development-emulator.html
Extract the packages and add the path to the repository in .bashrc

Install Intel OneAPI:
https://software.intel.com/content/www/us/en/develop/articles/installing-intel-oneapi-toolkits-via-apt.html

To build, use `make`
Run using Intel SDE
`sde64 -[spr|future] -- ./bin/amx N` where N is the dimension of the input square matrix. Two NxN matrix get initialized with randomly generated values and multiplied together
