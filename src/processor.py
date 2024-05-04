import cv2
from PIL import Image
import numpy as np
import os
from concurrent.futures import ProcessPoolExecutor
import time


def load_font_images(font_dir):
    font_images = {}
    for filename in os.listdir(font_dir):
        if filename.endswith('.png'):
            # filename is ASCII value of the character
            char = chr(int(filename.split('.')[0]))
            font_images[char] = np.array(Image.open(
                os.path.join(font_dir, filename)))
    return font_images


def compare_matrices(image_segment, font_images):
    min_distance = float('inf')
    best_match = None
    segment_array = np.array(image_segment)
    for char, font_image in font_images.items():
        distance = np.linalg.norm(segment_array - font_image)
        if distance < min_distance:
            min_distance = distance
            best_match = char, font_image
    return best_match


def process_image(img, font_images, font_size=20):
    width, height = img.size
    output_image = Image.new('L', (width, height))
    characters_grid = []

    for j in range(0, height, font_size):
        row_chars = []
        for i in range(0, width, font_size):
            segment = img.crop((i, j, i + font_size, j + font_size))
            best_match_char, best_match_img = compare_matrices(
                segment, font_images)
            output_image.paste(Image.fromarray(best_match_img), (i, j))
            row_chars.append(best_match_char)
        characters_grid.append(row_chars)

    return output_image, characters_grid


def process_frame(args):
    frame, count, font_images, font_size, output_img_dir, output_txt_dir = args
    img = Image.fromarray(cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY))
    output_image, characters_grid = process_image(img, font_images, font_size)

    frame_filename = f'frame_{str(count).zfill(10)}.png'
    text_filename = f'frame_{str(count).zfill(10)}.txt'

    output_image.save(os.path.join(output_img_dir, frame_filename))

    with open(os.path.join(output_txt_dir, text_filename), 'w') as file:
        for row in characters_grid:
            file.write(''.join(row) + '\n')

    print(f'Processed frame {count}')


def process_video(video_path, font_images, output_img_dir, output_txt_dir, font_size=20, max_workers=4):
    vidcap = cv2.VideoCapture(video_path)
    success, frame = vidcap.read()
    frames = []
    count = 0

    # Ensure directories exist
    if not os.path.exists(output_img_dir):
        os.makedirs(output_img_dir)
    if not os.path.exists(output_txt_dir):
        os.makedirs(output_txt_dir)

    # Prepare frames for processing
    while success:
        frames.append((frame, count, font_images, font_size,
                      output_img_dir, output_txt_dir))
        success, frame = vidcap.read()
        count += 1

    # Process frames in parallel
    with ProcessPoolExecutor(max_workers=max_workers) as executor:
        executor.map(process_frame, frames)

    vidcap.release()  # Properly release the video capture object


# Directory setup
font = 'ComicMono'
font_dir = f'fonts/{font}_chars'  # Directory containing the letter images
font_images = load_font_images(font_dir)

# Process a video
video_path = 'SampleVideo.mp4'
output_img_dir = 'output_images_py'
output_txt_dir = 'output_text_py'

start = time.time()
process_video(video_path, font_images, output_img_dir,
              output_txt_dir, font_size=10, max_workers=4)
end = time.time()
print("-------------------------------------------")
print("Video processing complete in Python3.")
print(f'Processing took {end - start:.2f} seconds')
