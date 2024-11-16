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
#include <iomanip>
#include <limits>
#include <chrono>

namespace fs = std::filesystem;
std::mutex io_mutex;

std::string formatNumber(int num, int length)
{
    std::ostringstream oss;
    oss << std::setw(length) << std::setfill('0') << num;
    return oss.str();
}

std::map<char, cv::Mat> load_font_images(const std::string &font_dir)
{
    std::map<char, cv::Mat> font_images;
    for (const auto &entry : fs::directory_iterator(font_dir))
    {
        if (entry.path().extension() == ".png")
        {
            std::string filename = entry.path().stem().string();
            int char_code;
            try
            {
                char_code = std::stoi(filename);
            }
            catch (const std::invalid_argument &ia)
            {
                std::cerr << "Invalid argument: " << ia.what() << '\n';
                continue;
            }
            catch (const std::out_of_range &oor)
            {
                std::cerr << "Out of Range error: " << oor.what() << '\n';
                continue;
            }

            if (char_code < 0 || char_code > 255)
            {
                std::cerr << "Character code out of valid range: " << char_code << '\n';
                continue;
            }

            char char_code_char = static_cast<char>(char_code);
            cv::Mat img = cv::imread(entry.path(), cv::IMREAD_GRAYSCALE);
            if (img.empty())
            {
                std::cerr << "Failed to load image for char code " << char_code_char << " at path " << entry.path() << std::endl;
                continue;
            }

            font_images[char_code_char] = img;
        }
    }
    return font_images;
}

std::pair<char, cv::Mat> compare_matrices(const cv::Mat &segment, const std::map<char, cv::Mat> &font_images)
{
    double min_distance = std::numeric_limits<double>::max();
    char best_match_char = 0;
    cv::Mat best_match_img;

    for (const auto &[char_code, font_image] : font_images)
    {
        if (!segment.empty() && !font_image.empty() && segment.type() == font_image.type() && segment.size() == font_image.size())
        {
            cv::Mat font_image_inv;
            cv::bitwise_not(font_image, font_image_inv); // For some reason, the images need to be inverted for the comparison to work
            cv::bitwise_not(segment, segment);           // Same thing here

            double distance = cv::norm(segment, font_image_inv, cv::NORM_L2);

            if (distance < min_distance)
            {
                min_distance = distance;
                best_match_char = char_code;
                best_match_img = font_image;
            }
        }
        else
        {
            std::cerr << "Incompatible or empty images for char " << char_code << std::endl;
        }
    }
    if (best_match_char <= 0 || best_match_char > 127)
    {
        std::cerr << "Invalid character match detected, using default." << std::endl;
        best_match_char = '?'; // Default character if no valid match found
    }
    return {best_match_char, best_match_img};
}

void process_frame(const cv::Mat &frame, int count, const std::map<char, cv::Mat> &font_images, int font_size, const std::string &output_img_dir, const std::string &output_txt_dir)
{
    cv::Mat gray_frame;
    cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);

    cv::Mat output_image = cv::Mat::zeros(gray_frame.size(), gray_frame.type());
    std::vector<std::string> characters_grid;

    for (int j = 0; j <= gray_frame.rows - font_size; j += font_size)
    {
        std::string row_chars;
        for (int i = 0; i <= gray_frame.cols - font_size; i += font_size)
        {
            cv::Rect region(i, j, font_size, font_size);
            cv::Mat segment = gray_frame(region);

            auto [best_match_char, best_match_img] = compare_matrices(segment, font_images);
            cv::Mat destination = output_image(cv::Rect(i, j, font_size, font_size));
            best_match_img.copyTo(destination);
            row_chars += best_match_char;
        }
        characters_grid.push_back(row_chars);
    }

    std::string frame_filename = output_img_dir + "/frame_" + formatNumber(count, 10) + ".png";
    std::string text_filename = output_txt_dir + "/frame_" + formatNumber(count, 10) + ".txt";

    bool isWritten = cv::imwrite(frame_filename, output_image);
    if (!isWritten)
    {
        std::cerr << "Failed to write image to " << frame_filename << std::endl;
    }

    std::ofstream file(text_filename);
    if (!file)
    {
        std::cerr << "Failed to open text file " << text_filename << std::endl;
    }
    else
    {
        for (const auto &row : characters_grid)
        {
            file << row << '\n';
        }
    }

    std::lock_guard<std::mutex> guard(io_mutex);
}

int main(int argc, char *argv[])
{
    auto start = std::chrono::high_resolution_clock::now();

    std::string video = "SampleVideo";
    std::string font = "ComicMono";
    int font_size = 10;

    try
    {
        if (argc > 1)
            font = argv[1];
        if (argc > 2)
            font_size = std::stoi(argv[2]);
        if (argc > 3)
            video = argv[3];
    }
    catch (const std::invalid_argument &ia)
    {
        std::cerr << "Invalid argument: " << ia.what() << '\n';
        return 1;
    }
    catch (const std::out_of_range &oor)
    {
        std::cerr << "Argument out of range: " << oor.what() << '\n';
        return 1;
    }

    std::string video_path = "videos/" + video + ".mp4";
    std::string output_img_dir = "output/frames";
    std::string output_txt_dir = "output/text";
    std::string font_dir = "fonts/" + font + "_chars";

    if (!fs::exists(output_img_dir))
        fs::create_directories(output_img_dir);
    if (!fs::exists(output_txt_dir))
        fs::create_directories(output_txt_dir);

    auto font_images = load_font_images(font_dir);

    cv::VideoCapture cap(video_path);
    if (!cap.isOpened())
    {
        std::cerr << "Error opening video file" << std::endl;
        return -1;
    }

    std::vector<std::thread> threads;
    cv::Mat frame;
    int count = 0;
    while (cap.read(frame))
    {
        std::cout << "Processing frame " << count << std::endl;

        threads.emplace_back(process_frame, frame.clone(), count, std::ref(font_images), font_size, output_img_dir, output_txt_dir);
        count++;
    }

    std::cout << "Joining threads..." << std::endl;

    for (auto &th : threads)
    {

        if (th.joinable())
            th.join();
    }

    cap.release();

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Video processing completed in C++." << std::endl;
    std::cout << "Processed " << count - 1 << " frames in " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " seconds." << std::endl;
    return 0;
}
