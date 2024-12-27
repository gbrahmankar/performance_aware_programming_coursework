# Compiler and flags
CXX = g++
CXXFLAGS = -DPROFILER -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-switch -std=c++14

# Source files
SRCS = main.cpp part_one/8086_decoder_simulator.cpp part_two/haversine_distance_problem.cpp part_two/haversine_distance_generator.cpp part_two/haversine_distance_calculator.cpp part_two/haversine_distance_parser.cpp part_three/part_three_main.cpp test/test.cpp common_utils.cpp

# Object files (replace .cpp with .o)
OBJS = $(SRCS:.cpp=.o)

# Target executable
TARGET = executable_file 

# Default rule
all: $(TARGET)

# Rule to create the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile each .cpp file into a .o file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule to remove compiled files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
