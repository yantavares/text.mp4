# text.mp4

**text.mp4** is a project that takes a video as input, converts each frame into ASCII art using any font you want, and generates a text-based video output along with the ASCII frames.

![Input Frame](public/frameExampleInput.png)

![Output Frame](public/frameExampleOutput.png)

## Requirements

- OpenCV4
- Python 3.x (for Python engine)
- C++ compiler (for C++ engine)
- GNU `make`

## Setup

1. Clone the repository:

   ```bash
   git clone https://github.com/yantavares/text.mp4.git
   cd text.mp4
   ```

2. Install OpenCV4 (for C++ engine):

   ```bash
   sudo apt-get install libopencv-dev
   ```

3. **(Optional)** Install required Python dependencies (for Python engine):

   ```bash
   pip install -r requirements.txt
   ```

4. **(Optional)** Add your desired video file to the `videos` directory, or use the provided `SampleVideo.mp4`.

5. **(Optional)** Add your desired font (.ttf files) to the `fonts` directory, or use the provided `ComicMono.ttf`.

## Usage

To run the project, use the `make` command:

```bash
make
```

### Steps after running `make`:

1. **Choose Engine**: C++ or Python.
   - The C++ engine is way faster, and so it is the recommended option.
   - Default is C++.
2. **Select Font**:
   - Default is `Comic Mono`.
3. **Choose Font Size**:
   - Choose a font size between 2 and 20.
   - Default is 10.
4. **Provide Video Filename**:
   - You will be prompted to choose a video file from the `videos` directory.
   - Default is `SampleVideo.mp4`.

_Observation_: If no input is provided, the default values will be used.

### Outputs

- The output will be available inside the `output` directory, which contains:
  - `frames/`: Each frame as an ASCII image.
  - `text/`: ASCII text representation of each frame as a text file.
  - `text.mp4`: The final text-based video output.

## Sample video example

![Video Example](public/sampletxt.GIF)

## License

This project is licensed under the MIT License.

## Contributing

Contributions are welcome! Please open an issue or a pull request for any changes or improvements.
