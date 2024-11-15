#!/bin/bash

# Directory containing your text frames
FRAME_DIR="output/text_resized"


# Display each frame in sequence
for frame in "$FRAME_DIR"/*.txt; do
    clear
    cat "$frame"
    sleep 0.06
done
