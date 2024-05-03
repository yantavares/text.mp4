CXX=g++
CXXFLAGS=-std=c++17
OPENCV=`pkg-config --cflags --libs opencv4`
TARGET=processor
SRC=processor.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(OPENCV)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
