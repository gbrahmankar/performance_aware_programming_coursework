# Compiler and flags
CXX = clang++
CXXFLAGS = -O3 -Wall -Wextra -Wno-unused -Wno-unused-parameter -Wno-switch -std=c++14 -DPROFILER 

# Source files
SRCS = main.cpp part_one/8086_decoder_simulator.cpp part_two/haversine_distance_problem.cpp part_two/haversine_distance_generator.cpp part_two/haversine_distance_calculator.cpp part_two/haversine_distance_parser.cpp part_three/part_three_main.cpp part_three/read_overhead_test_main.cpp part_three/read_overhead_tests.cpp part_three/repetition_tester.cpp part_three/incremental_page_touching.cpp part_three/decompose_x64_pointer.cpp part_three/backwards_page_touching.cpp part_three/front_end_test_main.cpp part_three/front_end_tests.cpp test/test.cpp common_utils.cpp 

OBJS = $(SRCS:.cpp=.o)

TARGET = executable_file 

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) fet_macho64.lib

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
