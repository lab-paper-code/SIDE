# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++17
PKG_FLAGS = $(shell pkg-config --cflags --libs libmongocxx)
LDFLAGS = -ls2 -lssl -lcrypto -lcpprest -lboost_system -lpthread

# Target and source files
TARGET = src/TRIE
SRCS = src/main.cpp src/ConfigParser.cpp
OBJS = $(SRCS:.cpp=.o)

# Header files (optional for dependency tracking)
HDRS = src/ConfigParser.hpp src/main.hpp

# Default build target
all: $(TARGET)

# Link target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(PKG_FLAGS) $(LDFLAGS)
	rm -f $(OBJS)

# Compile .cpp to .o
%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) $(PKG_FLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)