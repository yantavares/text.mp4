#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <filesystem>
#include <map>
#include <mutex>
#include <string>
#include <cmath>
#include <limits>

namespace fs = std::filesystem;
std::mutex io_mutex;

// Function to load font images
std::map<char, cv::Mat> load_font_images(const std::string &font_dir) {
    std::map<char, cv::Mat> font_images;
    for (const auto &entry : fs::directory_iterator(font_dir)) {
        if (entry.path().extension() == ".png") {
            char char_code = static_cast<char>(std::stoi(entry.path().stem()));
            font_images[char_code] = cv::imread(entry.path(), cv::IMREAD_GRAYSCALE);
        }
    }
    return font_images;
}

// Compare two matrices and find the one with the minimum Euclidean distance
std::pair<char, cv::Mat> compare_matrices(const cv::Mat &segment, const std::map<char, cv::Mat> &font_images) {
    double min_distance = std::numeric_limits<double>::max();
    char best_match_char = 0;
    cv::Mat best_match_img;

    for (const auto &[char_code, font_image] : font_images) {
        cv::Mat diff;
        cv::absdiff(segment, font_image, diff);
        double distance = cv::norm(diff, cv::NORM_L2);
        if (distance < min_distance) {
            min_distance = distance;
            best_match_char = char_code;
            best_match_img = font_image;
        }
    }
    return {best_match_char, best_match_img};
}

// Process a single frame
void process_frame(const cv::Mat &frame, int count, const std::map<char, cv::Mat> &font_images, int font_size, const std::string &output_img_dir, const std::string &output_txt_dir) {
    cv::Mat gray_frame;
    cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);

    cv::Mat output_image = cv::Mat::zeros(gray_frame.size(), gray_frame.type());
    std::vector<std::string> characters_grid;

    for (int j = 0; j <= gray_frame.rows - font_size; j += font_size) {
        std::string row_chars;
        for (int i = 0; i <= gray_frame.cols - font_size; i += font_size) {
            cv::Rect region(i, j, font_size, font_size);
            cv::Mat segment = gray_frame(region);

            auto [best_match_char, best_match_img] = compare_matrices(segment, font_images);
            cv::Mat destination = output_image(cv::Rect(i, j, font_size, font_size));
            best_match_img.copyTo(destination);
            row_chars += best_match_char;
        }
        characters_grid.push_back(row_chars);
    }

    std::string frame_filename = output_img_dir + "/frame_" + std::to_string(count) + ".png";
    std::string text_filename = output_txt_dir + "/frame_" + std::to_string(count) + ".txt";

    cv::imwrite(frame_filename, output_image);
    std::ofstream file(text_filename);
    for (const auto &row : characters_grid) {
        file << row << '\n';
    }

    std::lock_guard<std::mutex> guard(io_mutex);
    std::cout << "Processed frame " << count << std::endl;
}

// Main driver function
int main() {
    std::string video_path = "SampleVideo.mp4";
    std::string output_img_dir = "output_images";
    std::string output_txt_dir = "output_text";
    int font_size = 10;
    std::string font_dir = "fonts/ComicMono_chars";

    if (!fs::exists(output_img_dir))
        fs::create_directories(output_img_dir);
    if (!fs::exists(output_txt_dir))
        fs::create_directories(output_txt_dir);

    auto font_images = load_font_images(font_dir);

    cv::VideoCapture cap(video_path);
    if (!cap.isOpened()) {
        std::cerr << "Error opening video file" << std::endl;
        return -1;
    }

    std::vector<std::thread> threads;
    cv::Mat frame;
    int count = 0;
    while (cap.read(frame)) {
        if (count > 100) break;  // Limit to 100 frames to manage memory usage
        threads.emplace_back(process_frame, frame.clone(), count, std::ref(font_images), font_size, output_img_dir, output_txt_dir);
        count++;
    }

    for (auto &th : threads) {
        if (th.joinable()) th.join();
    }

    cap.release();
    std::cout << "Video processing completed." << std::endl;
    return 0;
}
