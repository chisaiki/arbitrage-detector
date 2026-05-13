# arbitrage-detector

## Current Build Instructions
cd Project
g++ -o benchmark benchmark.cpp

 - You can see the stats of the MSPCQueue implementation

## Build Instructions
The project includes a CMakeLists.txt. Create a build folder in the Project directory, then run cmake. See instructions below.

```bash
cd Project          # move into your project folder
mkdir build         # create a new empty folder called build
cd build            # move into that build folder
cmake ..            # run cmake, pointing it at the parent folder
make                # create the executable file
./main_exe          # run the program
```