MAKEFLAGS += --no-print-directory
CXX = g++
CXXFLAGS = -std=c++17
OPENCV = `pkg-config --cflags --libs opencv4`
TARGET = processor
SRCDIR = src
CPPSRC = $(SRCDIR)/processor.cpp
BINDIR = bin
OUTPUTDIR = output
FONT = ComicMono
VIDEO = SampleVideo
FONTSIZE = 11
PYTHON = python3
UTLSCRIPT1 = $(SRCDIR)/utils/font_generator.py
PLAY_SCRIPT = ./play.sh

.PHONY: all choose run-cpp play clean install

all: clean choose

choose:
	@echo "--------------------------"
	@echo "| Processing Engine Menu |"
	@echo "--------------------------"
	@read -p "Enter the font name [default: ComicMono]: " font; \
	read -p "Enter video name [default: SampleVideo]: " video; \
	read -p "Enter font size (WARNING: Must be equal to terminal font size!) [default: 11]: " fontsize; \
	font=$${font:-$(FONT)}; \
	video=$${video:-$(VIDEO)}; \
	fontsize=$${fontsize:-$(FONTSIZE)}; \
	echo "Selected font: $$font, font size: $$fontsize, video name: $$video"; \
	$(MAKE) run-cpp FONT="$$font" VIDEO="$$video" FONTSIZE="$$fontsize"

$(BINDIR)/$(TARGET): $(CPPSRC)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(OPENCV)

run-cpp: $(BINDIR)/$(TARGET)
	@echo "Running C++ program with font: '$(FONT)', font size: '$(FONTSIZE)', and video: '$(VIDEO)'"
	@$(PYTHON) $(UTLSCRIPT1) "$(FONT)" "$(FONTSIZE)"
	@./$(BINDIR)/$(TARGET) "$(FONT)" "$(FONTSIZE)" "$(VIDEO)" 
	$(MAKE) play

play:
	@$(PLAY_SCRIPT)

clean:
	@rm -rf $(OUTPUTDIR)
