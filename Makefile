CXX = g++
CXXFLAGS = -std=c++17
OPENCV = $(shell pkg-config --cflags --libs opencv4)
TARGET = processor
SRCDIR = src
SRC = $(SRCDIR)/processor.cpp
BINDIR = bin
PYTHON = python3
PYSCRIPT = src/utils/video_generator.py

# Creates bin directory if it doesn't exist and then compile the program
all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(SRC)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(OPENCV)

run: $(BINDIR)/$(TARGET)
	@./$<
	@$(PYTHON) $(PYSCRIPT)

clean:
	@rm -rf $(BINDIR)

.PHONY: all run clean
