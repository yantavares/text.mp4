import cv2
from PIL import Image
import numpy as np
import os


def load_font_images(font_dir):
    font_images = {}
    for filename in os.listdir(font_dir):
        if filename.endswith('.png'):
            # Assuming filename is ASCII value of the character
            char = chr(int(filename.split('.')[0]))
            font_images[char] = np.array(Image.open(
                os.path.join(font_dir, filename)).convert('RGB'))
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
    output_image = Image.new('RGB', (width, height))
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


def process_video(video_path, font_images, output_img_dir, output_txt_dir, font_size=20):
    vidcap = cv2.VideoCapture(video_path)
    success, frame = vidcap.read()
    count = 0

    if not os.path.exists(output_img_dir):
        os.makedirs(output_img_dir)
    if not os.path.exists(output_txt_dir):
        os.makedirs(output_txt_dir)

    while success:
        img = Image.fromarray(cv2.cvtColor(frame, cv2.COLOR_BGR2RGB))
        output_image, characters_grid = process_image(
            img, font_images, font_size)

        print(f'Processing frame {count}')
        frame_filename = f'frame_{str(count).zfill(10)}.png'
        text_filename = f'frame_{str(count).zfill(10)}.txt'

        output_image.save(os.path.join(output_img_dir, frame_filename))

        with open(os.path.join(output_txt_dir, text_filename), 'w') as file:
            for row in characters_grid:
                file.write(''.join(row) + '\n')

        success, frame = vidcap.read()
        count += 1

        if count > 100:
            break


# Directory setup
font_dir = 'cs/font'  # Directory containing the letter images
font_images = load_font_images(font_dir)

# Process a video
video_path = 'SampleVideo.mp4'
output_img_dir = 'output_images'
output_txt_dir = 'output_text'
process_video(video_path, font_images, output_img_dir, output_txt_dir, 10)
