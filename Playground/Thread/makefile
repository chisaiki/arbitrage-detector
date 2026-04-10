# Define the compiler
CXX = g++

# Compiler flags (add -pthread here)
CXXFLAGS = -Wall -std=c++17 -pthread 

# Linker flags (add -pthread here too)
LDFLAGS = -pthread

# Your target executable
TARGET = miniarbtirage
SRC = miniarbtirage.cpp

# The build rule
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Clean rule (optional but good practice)
clean:
	rm -f $(TARGET)