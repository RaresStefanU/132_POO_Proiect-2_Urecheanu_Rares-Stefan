CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Iinclude
TARGET   = septica

SRCS = src/main.cpp \
       src/carte.cpp \
       src/pachet.cpp \
       src/jucator.cpp \
       src/joc.cpp

OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
