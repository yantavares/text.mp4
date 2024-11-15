# video.txt

**video.txt** is a project that takes a video as input, converts each frame into ASCII art using any font you want, and generates a text-based video output inside your terminal along with the ASCII frames.

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

To run the generated text-based video in the terminal, use the `make play` command:

```bash
   make play
```

Observation: The `make play` command will only work on UNIX systems and will automatically resize the video to fit terminal size.

### Steps after running `make`:

1. **Select Font**:
   - Default is `Comic Mono`.
2. **Choose Font Size**:
   - Choose a font size between 2 and 20.
   - Default is 10.
3. **Provide Video Filename**:
   - You will be prompted to choose a video file from the `videos` directory.
   - Default is `SampleVideo.mp4`.

_Observation_: If no input is provided, the default values will be used.

### Outputs

- The output will be available inside the `output` directory, which contains the `txt` files generated.

## License

This project is licensed under the MIT License.

## Contributing

Contributions are welcome! Please open an issue or a pull request for any changes or improvements.
