# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2

# Build targets
TARGET = TinyJob
SOURCES = Job.cpp main.cpp Worker.cpp JobSystem.cpp JobQueue.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = Job.h Worker.h JobSystem.h JobQueue.h

# Default target
all: $(TARGET)

# Linking all objects to executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compiling each source file - adding dependency on headers
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
