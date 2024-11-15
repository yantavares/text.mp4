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
FONTSIZE = 10
PYTHON = python3
UTLSCRIPT1 = $(SRCDIR)/utils/font_generator.py
UTLSCRIPT2 = $(SRCDIR)/utils/video_generator.py
PLAY_SCRIPT = ./play.sh

.PHONY: all choose run-cpp play clean install

all: choose

choose:
	@echo "--------------------------"
	@echo "| Processing Engine Menu |"
	@echo "--------------------------"
	@read -p "Enter the font name [default: ComicMono]: " font; \
	read -p "Enter video name [default: SampleVideo]: " video; \
	read -p "Enter font size (2-20) [default: 10]: " fontsize; \
	read -p "Play the video in terminal? Y/N [default: Y]: " should_play; \
	font=$${font:-$(FONT)}; \
	video=$${video:-$(VIDEO)}; \
	fontsize=$${fontsize:-$(FONTSIZE)}; \
	should_play=$${should_play:-Y}; \
	echo "Selected font: $$font, font size: $$fontsize, video name: $$video"; \
	$(MAKE) run-cpp FONT="$$font" VIDEO="$$video" FONTSIZE="$$fontsize" SHOULD_PLAY="$$should_play"

$(BINDIR)/$(TARGET): $(CPPSRC)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(OPENCV)

run-cpp: $(BINDIR)/$(TARGET)
	@echo "Running C++ program with font: '$(FONT)', font size: '$(FONTSIZE)', and video: '$(VIDEO)'"
	@$(PYTHON) $(UTLSCRIPT1) "$(FONT)" "$(FONTSIZE)"
	@./$(BINDIR)/$(TARGET) "$(FONT)" "$(FONTSIZE)" "$(VIDEO)" "$(SHOULD_PLAY)"
	@$(PYTHON) $(UTLSCRIPT2) "$(VIDEO)" "$(FONT)" "$(FONTSIZE)"
	@if [ "$(SHOULD_PLAY)" = "Y" ] || [ "$(SHOULD_PLAY)" = "y" ]; then \
		$(MAKE) play; \
	fi
	@echo "Done! Full video (in native dimensions) can be found at 'ouput/text.mp4'"

play:
	@$(PLAY_SCRIPT)

clean:
	@rm -rf $(BINDIR) \
	rm -rf $(OUTPUTDIR)
