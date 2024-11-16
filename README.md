# video.txt

**video.txt** is a project that takes a video as input, converts each frame into ASCII art using any font you want, and offers two output modes:

1. Plays a text-based video output inside your terminal.
2. Saves the ASCII frames as a rendered `.mp4` file in the `output` directory.

**Example** (takes a few seconds to load):
![Video Example](public/sampletxt.gif)

## Requirements

- GNU `make`
- OpenCV4 (`libopencv-dev` in Ubuntu, `opencv` in Fedora and Arch)
- g++ compiler (for C++ engine)
- vtk, glew, fmt (if not installed already)

## Setup (UNIX systems)

1. Clone the repository:

```bash
  git clone https://github.com/yantavares/video.txt.git
  cd video.txt
```

2. If necessary, make `play.sh` executable:

```bash
  chmod +x play.sh
```

3. **(Optional)** Add your desired video file to the `videos` directory, or use the provided `SampleVideo.mp4`.

4. **(Optional)** Add your desired font (.ttf files) to the `fonts` directory, or use the provided `ComicMono.ttf`.

## **Running the Project**

To run the project, use the `make` command:

```bash
   make
```

You will be prompted to choose between two modes:

1. **Play in Terminal**: Displays the ASCII-art-based video inside your terminal.
2. **Save as MP4**: Generates a `.mp4` file in the `output` directory, rendering the ASCII-art-based video.

After selecting your mode, follow the steps below.

### Steps After Running `make`:

1. **Select Font**:
   - Default is `Comic Mono`.
2. **Choose Font Size**:
   - **Must match the terminal font size**.
   - Choose a font size between 2 and 20.
   - Default is 11.
3. **Provide Video Filename**:
   - You will be prompted to choose a video file from the `videos` directory.
   - Default is `SampleVideo.mp4`.
4. **Choose Mode**:
   - Select one of the following:
     - **1**: Play the video in the terminal.
     - **2**: Save the ASCII-rendered video as an `.mp4` file in the `output` directory.

If no input is provided during any prompt, the default values will be used.

### Commands

- To **play the video in your terminal** after running `make`:

```bash
   make play
```

The video will automatically resize to fit your terminal dimensions.

- To **save the ASCII-rendered video as an MP4**:

```bash
   make
```

Choose mode `2` when prompted, and the `.mp4` file will be saved in the `output` directory.

### Outputs

1. If you select **Mode 1**, the ASCII video will play directly in your terminal.
2. If you select **Mode 2**, the ASCII-rendered frames will be compiled into a `.mp4` file, saved as `output/text.mp4`.

Additionally, all individual ASCII frames are saved as `.txt` files in the `output` directory.

## License

This project is licensed under the MIT License.

## Contributing

Contributions are welcome! Please open an issue or a pull request for any changes or improvements.
