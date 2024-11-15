#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <map>
#include <mutex>
#include <string>
#include <cmath>
#include <iomanip>
#include <limits>
#include <chrono>
#include <thread>
#include <queue>
#include <condition_variable>
#include <sys/ioctl.h>
#include <unistd.h>
#include <atomic>

namespace fs = std::filesystem;
std::mutex io_mutex;

class ThreadPool
{
public:
    ThreadPool(size_t threads);
    ~ThreadPool();
    template <class F>
    void enqueue(F &&f);

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

ThreadPool::ThreadPool(size_t threads) : stop(false)
{
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back([this]
                             {
            for (;;) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                    if (this->stop && this->tasks.empty())
                        return;
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                task();
            } });
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers)
        worker.join();
}

template <class F>
void ThreadPool::enqueue(F &&f)
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.emplace(std::forward<F>(f));
    }
    condition.notify_one();
}

std::pair<int, int> get_terminal_size()
{
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
    {
        return {w.ws_col, w.ws_row};
    }
    else
    {
        std::cerr << "Unable to detect terminal size. Defaulting to 80x24." << std::endl;
        return {80, 24};
    }
}

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

char compare_matrices(const cv::Mat &segment, const std::map<char, cv::Mat> &font_images)
{
    double min_distance = std::numeric_limits<double>::max();
    char best_match_char = 0;

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
    return best_match_char;
}

void process_frame(const cv::Mat &frame, int count, const std::map<char, cv::Mat> &font_images, int font_size, const std::string &output_txt_dir)
{
    cv::Mat gray_frame;

    auto [terminal_width, terminal_height] = get_terminal_size();
    cv::Mat resized_frame;

    cv::resize(frame, resized_frame, cv::Size(terminal_width * font_size, terminal_height * font_size));
    cvtColor(resized_frame, gray_frame, cv::COLOR_BGR2GRAY);

    std::vector<std::string> characters_grid;

    for (int j = 0; j <= gray_frame.rows - font_size; j += font_size)
    {
        std::string row_chars;
        for (int i = 0; i <= gray_frame.cols - font_size; i += font_size)
        {
            cv::Rect region(i, j, font_size, font_size);
            cv::Mat segment = gray_frame(region);

            char best_match_char = compare_matrices(segment, font_images);
            row_chars += best_match_char;
        }
        characters_grid.push_back(row_chars);
    }

    std::string text_filename = output_txt_dir + "/frame_" + formatNumber(count, 10) + ".txt";

    std::ofstream file(text_filename);
    if (!file)
    {
        std::cerr << "Failed to open text file " << text_filename << std::endl;
        return;
    }

    for (const auto &row : characters_grid)
    {
        file << row << '\n';
    }
}

int main(int argc, char *argv[])
{
    auto start = std::chrono::high_resolution_clock::now();

    std::string video;
    std::string font;
    int font_size;

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
    std::string output_txt_dir = "output";
    std::string font_dir = "fonts/" + font + "_chars";

    if (!fs::exists(output_txt_dir))
        fs::create_directories(output_txt_dir);

    auto font_images = load_font_images(font_dir);

    cv::VideoCapture cap(video_path);
    if (!cap.isOpened())
    {
        std::cerr << "Error opening video file" << std::endl;
        return -1;
    }

    ThreadPool pool(std::thread::hardware_concurrency());
    std::atomic<int> completed_tasks{0};

    cv::Mat frame;
    int count = 0;

    while (cap.read(frame))
    {
        cv::Mat frame_copy = frame.clone();
        int current_count = count++;

        pool.enqueue([=, &completed_tasks]()
                     {
            {
                std::lock_guard<std::mutex> guard(io_mutex);
                std::cout << "Processing frame " << current_count << std::endl;
            }
            process_frame(frame_copy, current_count, std::ref(font_images), font_size, output_txt_dir);
            completed_tasks.fetch_add(1, std::memory_order_relaxed); });
    }

    cap.release();

    while (completed_tasks.load(std::memory_order_relaxed) < count)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Video processing completed in C++." << std::endl;
    std::cout << "Processed " << count << " frames in "
              << std::chrono::duration_cast<std::chrono::seconds>(end - start).count()
              << " seconds." << std::endl;
    return 0;
}
