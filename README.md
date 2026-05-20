# arbitrage-detector

Currently you can only run the test files in Project/multithreadmain.cpp to see how it runs against a custom mutex based std::queue implementation. 

$ g++ -o multithread_t multithreadmain.cpp  
$ ./multithread_t

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
