MAKEFLAGS += --no-print-directory
CXX = g++
CXXFLAGS = -std=c++17
OPENCV = `pkg-config --cflags --libs opencv4`
TARGET = processor
SRCDIR = src
CPPSRC = $(SRCDIR)/processor.cpp
PYSCRIPT = $(SRCDIR)/processor.py
BINDIR = bin
FONT = ComicMono
VIDEO = SampleVideo
FONTSIZE = 10  # Default font size
PYTHON = python3
UTLSCRIPT1 = $(SRCDIR)/utils/font_generator.py
UTLSCRIPT2 = $(SRCDIR)/utils/video_generator.py

.PHONY: all choose run-cpp run-python clean install

all: choose

choose:
	@echo "--------------------------"
	@echo "| Processing Engine Menu |"
	@echo "--------------------------"
	@echo "0) C++ (default)"
	@echo "1) Python"
	@echo "--------------------------"
	@read -p "Enter your choice and press [ENTER] [default: C++]: " engine; \
	read -p "Enter the font name [default: ComicMono]: " font; \
	read -p "Enter video name [default: SampleVideo]: " video; \
	read -p "Enter font size (2-20) [default: 10]: " fontsize; \
	font=$${font:-$(FONT)}; \
	video=$${video:-$(VIDEO)}; \
	fontsize=$${fontsize:-$(FONTSIZE)}; \
	echo "Selected font: $$font, font size: $$fontsize, video name: $$video"; \
	if [ "$$engine" = "1" ]; then \
		$(MAKE) run-python FONT="$$font" VIDEO="$$video" FONTSIZE="$$fontsize"; \
	elif [ "$$engine" = "0" ] || [ -z "$$engine" ]; then \
		$(MAKE) run-cpp FONT="$$font" VIDEO="$$video" FONTSIZE="$$fontsize"; \
	else \
		echo "Invalid choice. Using default C++." && \
		$(MAKE) run-cpp FONT="$$font" VIDEO="$$video" FONTSIZE="$$fontsize"; \
	fi


$(BINDIR)/$(TARGET): $(CPPSRC)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(OPENCV)

run-cpp: $(BINDIR)/$(TARGET)
	@echo "Running C++ program with font: '$(FONT)', font size: '$(FONTSIZE)', and video: '$(VIDEO)'"
	@$(PYTHON) $(UTLSCRIPT1) "$(FONT)" "$(FONTSIZE)"
	@./$(BINDIR)/$(TARGET) "$(FONT)" "$(FONTSIZE)" "$(VIDEO)"
	@$(PYTHON) $(UTLSCRIPT2) "$(VIDEO)" "$(FONT)" "$(FONTSIZE)"

run-python:
	@echo "Running Python program with font: '$(FONT)', font size: '$(FONTSIZE)', and video: '$(VIDEO)'"
	@$(PYTHON) $(UTLSCRIPT1) "$(FONT)" "$(FONTSIZE)"
	@$(PYTHON) $(PYSCRIPT) "$(FONT)" "$(FONTSIZE)" "$(VIDEO)"
	@$(PYTHON) $(UTLSCRIPT2) "$(FONT)" "$(FONTSIZE)" "$(VIDEO)"

clean:
	@rm -rf $(BINDIR)

install:
	@echo "Installing necessary packages..."
	@if ! command -v make >/dev/null 2>&1; then \
		echo "Installing make..."; \
		sudo apt-get install -y make; \
	else \
		echo "Make is already installed"; \
	fi
	@if ! command -v $(CXX) >/dev/null 2>&1; then \
		echo "Installing g++..."; \
		sudo apt-get install -y g++; \
	else \
		echo "g++ is already installed"; \
	fi
	@if ! dpkg -s libopencv-dev >/dev/null 2>&1; then \
		echo "Installing libopencv-dev..."; \
		sudo apt-get install -y libopencv-dev; \
	else \
		echo "libopencv-dev is already installed"; \
	fi
	@if ! command -v $(PYTHON) >/dev/null 2>&1; then \
		echo "Error: Python3 is not installed. You can install it by running the command:"; \
		echo "  sudo apt-get install -y python3"; \
	else \
		echo "Python3 is already installed"; \
	fi
	@if ! $(PYTHON) -m pip >/dev/null 2>&1; then \
		echo "Error: pip for Python3 is not installed. You can install it by running the command:"; \
		echo "  sudo apt-get install -y python3-pip"; \
	else \
		echo "pip for Python3 is already installed"; \
		$(PYTHON) -m pip install -r requirements.txt; \
	fi
