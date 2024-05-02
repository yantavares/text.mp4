from PIL import Image, ImageDraw, ImageFont
import os


def textsize(text, font):
    """Get dimensions of the given text when rendered in the given font."""
    im = Image.new(mode="L", size=(0, 0))
    draw = ImageDraw.Draw(im)
    _, _, width, height = draw.textbbox((0, 0), text=text, font=font)
    return width, height


def create_images_from_font(font_path, output_dir, characters, image_size=(10, 10), font_size=10):
    # Load the font
    font = ImageFont.truetype(font_path, font_size)

    # Create output directory if it doesn't exist
    if os.path.exists(output_dir):
        os.removedirs(output_dir)

    os.makedirs(output_dir)

    # Generate an image for each character
    for char in characters:
        img = Image.new('L', image_size, color=255)  # white background
        d = ImageDraw.Draw(img)
        # Calculate text position: centered
        width, height = textsize(char, font)  # Using updated textsize function
        position = ((image_size[0] - width) / 2, (image_size[1] - height) / 2)
        d.text(position, char, font=font, fill=0)  # black text

        # Save the image using ASCII value to avoid file name issues with special characters
        img.save(os.path.join(output_dir, f"{ord(char)}.png"))


# Usage example
font_path = 'cs/ComicMono.ttf'  # Path to the TTF font file
output_dir = 'cs/font'  # Directory to save letter images
# Characters to render
characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,!?-()[]{}<>:;\'"/\\@#$%^&*~`+=_|<>&'
create_images_from_font(font_path, output_dir, characters)
