# arbitrage-detector

Currently you can only run the test files in Project/multithreadmain.cpp to see how it runs against a custom mutex based std::queue implementation. 

$ cd Project/Testing  
$ g++ -std=c++17 -O2 -pthread benchmark.cpp -o benchmark (IF ON WSL or any Linux Based System)   
$ ./benchmark  

NOTE: There is a bug that can sometimes cause a deadlock within the threads, so you may have to run the executable multiple times. 

## Build Instructions [Websockets aren't in the update yet so please ignore this]
The project includes a CMakeLists.txt. Create a build folder in the Project directory, then run cmake. See instructions below.

```bash
cd Project          # move into your project folder
mkdir build         # create a new empty folder called build
cd build            # move into that build folder
cmake ..            # run cmake, pointing it at the parent folder
make                # create the executable file
./main_exe          # run the program
```
