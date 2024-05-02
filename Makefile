CXX = g++

CXXFLAGS = -std=c++17 -O2 -Wall -I/usr/include/boost -I/usr/local/include/gtest
LDFLAGS = -lboost_program_options -lgtest -lgtest_main -lpthread

# Исходные файлы
SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = app

# Путь к исходным файлам тестов
TEST_SOURCES = $(wildcard tests/*.cpp)
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)
TEST_EXECUTABLE = run_tests

all: $(EXECUTABLE) $(TEST_EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

$(TEST_EXECUTABLE): $(TEST_OBJECTS)
	$(CXX) $(TEST_OBJECTS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -f $(EXECUTABLE) $(TEST_EXECUTABLE) $(OBJECTS) $(TEST_OBJECTS)

.PHONY: all clean