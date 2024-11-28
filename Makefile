CXX = g++
CXXFLAGS = -Wall -Wextra -pthread -std=c++11
TARGET = chat
SRCS = main.cpp server.cpp client.cpp base.cpp
OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(CXXFLAGS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean