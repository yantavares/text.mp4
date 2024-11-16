MAKEFLAGS += --no-print-directory
CXX = g++
CXXFLAGS = -std=c++17
OPENCV = `pkg-config --cflags --libs opencv4`
TARGET = processor
SRCDIR = src
CPPSRC = $(SRCDIR)/processor.cpp
CPPSRC2 = $(SRCDIR)/video_processor.cpp
BINDIR = bin
OUTPUTDIR = output
FONT = ComicMono
VIDEO = SampleVideo
MODE = 1
FONTSIZE = 11
PYTHON = python3
UTLSCRIPT1 = $(SRCDIR)/utils/font_generator.py
UTLSCRIPT2 = $(SRCDIR)/utils/video_generator.py
PLAY_SCRIPT = ./play.sh

.PHONY: all choose run-cpp play clean install

all: clean choose

choose:
	@echo "--------------------------"
	@echo "| Processing Engine Menu |"
	@echo "--------------------------"
	@echo ""
	@echo "!! Make terminal fullscreen for better results. !!"
	@echo ""
	@read -p "Enter the font name [default: ComicMono]: " font; \
	read -p "MODE: Play in Terminal (1) | Save mp4 file (2) [default: 1]: " mode; \
	read -p "Enter video name [default: SampleVideo]: " video; \
	read -p "Enter font size [default: 11]: " fontsize; \
	@if ["$(MODE)" = "1"] then \
		echo "(WARNING: Must be equal to terminal font size!)" \
	fi; \
	font=$${font:-$(FONT)}; \
	video=$${video:-$(VIDEO)}; \
	fontsize=$${fontsize:-$(FONTSIZE)}; \
	mode=$${mode:-$(MODE)}; \
	echo "Selected font: $$font, font size: $$fontsize, video name: $$video, mode: $$mode"; \
	$(MAKE) run-cpp FONT="$$font" VIDEO="$$video" FONTSIZE="$$fontsize" MODE="$$mode"

$(BINDIR)/$(TARGET): $(CPPSRC) $(CPPSRC2)
	@mkdir -p $(BINDIR)
	@if [ "$(MODE)" = "1" ]; then \
		$(CXX) $(CXXFLAGS) -o $@ $(CPPSRC) $(OPENCV); \
	else \
		$(CXX) $(CXXFLAGS) -o $@ $(CPPSRC2) $(OPENCV); \
	fi

run-cpp: $(BINDIR)/$(TARGET)
	@echo "Running C++ program with font: '$(FONT)', font size: '$(FONTSIZE)', video: '$(VIDEO)'"
	@$(PYTHON) $(UTLSCRIPT1) "$(FONT)" "$(FONTSIZE)"
	@./$(BINDIR)/$(TARGET) "$(FONT)" "$(FONTSIZE)" "$(VIDEO)" 
	@if [ "$(MODE)" = "1" ]; then \
		$(MAKE) play; \
	else \
		$(PYTHON) $(UTLSCRIPT2) "$(VIDEO)" "$(FONT)" "$(FONTSIZE)"; \
		echo "Done! Full video (in native dimensions) can be found at '$(OUTPUTDIR)/text.mp4'"; \
	fi

play:
	@$(PLAY_SCRIPT)

clean:
	@rm -rf $(OUTPUTDIR)
	@rm -rf $(BINDIR)
