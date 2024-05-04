import cv2
import os


def create_video_from_frames(frame_dir, output_video_path, fps=24):
    # Get the list of frame files sorted by frame number
    frame_files = [os.path.join(frame_dir, f) for f in sorted(
        os.listdir(frame_dir)) if f.endswith('.png')]
    # Read the first frame to determine the size
    frame = cv2.imread(frame_files[0])
    height, width, layers = frame.shape

    # Define the codec and create a VideoWriter object
    fourcc = cv2.VideoWriter_fourcc(*'mp4v')  # You can also use 'XVID'
    video = cv2.VideoWriter(output_video_path, fourcc, fps, (width, height))

    # Read each file and add it to the video
    for file in frame_files:
        frame = cv2.imread(file)
        video.write(frame)  # Add frame to video

    video.release()  # Release the video writer


# Directory and video settings
frame_dir = 'output/frames'  # Directory containing frames
# Desired path for the output video
output_video_path = 'output/videos/output_video.mp4'
fps = 24  # Frames per second of the output video

create_video_from_frames(frame_dir, output_video_path, fps)
