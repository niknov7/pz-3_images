#include "PGMImage.h"

#include <algorithm>
#include <random>


PGMImage::PGMImage() : width(0), height(0), maxVal(255) {
}

PGMImage::PGMImage(const std::string &filename) {
    if (!load(filename)) {
        std::cerr << "Failed to load image: " << filename << std::endl;
    }
}

bool PGMImage::load(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    std::string format;
    file >> format;
    if (format != "P2") {
        std::cerr << "Unsupported format in file: " << filename << std::endl;
        return false;
    }

    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    while (file.peek() == '#') {
        std::string comment;
        getline(file, comment);
    }

    file >> width >> height >> maxVal;
    pixels.resize(width * height);

    for (int i = 0; i < width * height; ++i) {
        file >> pixels[i];
    }

    std::cout << "File " << filename << " successfully loaded!" << std::endl;
    return true;
}

bool PGMImage::save(const std::string &filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to save file: " << filename << std::endl;
        return false;
    }

    file << "P2\n";
    file << width << " " << height << "\n";
    file << maxVal << "\n";

    for (int i = 0; i < width * height; ++i) {
        file << pixels[i] << '\n';
    }

    std::cout << "File " << filename << " successfully saved!" << std::endl;
    return true;
}

std::pair<int, int> PGMImage::get_size() const {
    return {width, height};
}

unsigned short &PGMImage::operator()(const int row, const int col) {
    if (row < 0 || row >= height || col < 0 || col >= width)
        throw std::out_of_range("Pixel coordinates out of range");
    return pixels[row * width + col];
}

const unsigned short &PGMImage::operator()(const int row, const int col) const {
    if (row < 0 || row >= height || col < 0 || col >= width)
        throw std::out_of_range("Pixel coordinates out of range");
    return pixels[row * width + col];
}


void PGMImage::add_uniform_noise(const double levelPercent) {
    if (levelPercent <= 0.0) return;

    std::random_device rd;
    std::mt19937 gen(rd());

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int original = (*this)(i, j);
            int delta = static_cast<int>(original * levelPercent / 100.0);

            std::uniform_int_distribution<> dis(-delta, delta);
            (*this)(i, j) = static_cast<unsigned short>(
                std::clamp(original + dis(gen), 0, static_cast<int>(maxVal))
            );
        }
    }
}

void PGMImage::add_gaussian_noise(const double sigmaPercent) {
    if (sigmaPercent <= 0.0) return;

    std::random_device rd;
    std::mt19937 gen(rd());

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int original = (*this)(i, j);

            double sigma = original * sigmaPercent / 100.0;

            std::normal_distribution<double> dist(0.0, sigma);

            int noisy = static_cast<int>(original + dist(gen));

            (*this)(i, j) = static_cast<unsigned short>(std::clamp(noisy, 0, static_cast<int>(maxVal)));
        }
    }
}


void PGMImage::add_salt_and_pepper_noise(const double probability) {
    if (probability <= 0.0) return;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    std::uniform_int_distribution<> salt_pepper(0, 1);


    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (dis(gen) < probability) {
                (*this)(i, j) = salt_pepper(gen) ? maxVal : 0;
            }
        }
    }
}

void PGMImage::median_filter(const int windowSize) {
    if (windowSize % 2 == 0 || windowSize < 3)
        throw std::invalid_argument("Window size must be odd and >= 3");

    int offset = windowSize / 2;

    std::vector<unsigned short> copy = pixels;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            std::vector<unsigned short> neighbors;

            for (int di = -offset; di <= offset; ++di) {
                for (int dj = -offset; dj <= offset; ++dj) {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < height && nj >= 0 && nj < width) {
                        neighbors.push_back(copy[ni * width + nj]);
                    }
                }
            }

            std::sort(neighbors.begin(), neighbors.end());
            unsigned short median = neighbors[neighbors.size() / 2];
            (*this)(i, j) = median;
        }
    }
}

void PGMImage::exponential_moving_average(const double k) {
    if (k <= 0.0 || k > 1.0) throw std::invalid_argument("k must be 0 < k <= 1");
    std::vector<unsigned short> newPixels = pixels;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = y * width + x;
            if (idx == 0) continue;
            newPixels[idx] = static_cast<unsigned short>(
                pixels[idx] * k + newPixels[idx - 1] * (1.0 - k)
            );
        }
    }

    pixels = newPixels;
}

void PGMImage::moving_average_filter(const int size) {
    if (size <= 1) return;
    std::vector<unsigned short> newPixels = pixels;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int sum = 0;
            int count = 0;
            for (int dy = -size / 2; dy <= size / 2; ++dy) {
                for (int dx = -size / 2; dx <= size / 2; ++dx) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        sum += (*this)(ny, nx);
                        count++;
                    }
                }
            }
            newPixels[y * width + x] = static_cast<unsigned short>(sum / count);
        }
    }

    pixels = newPixels;
}

unsigned short PGMImage::get_max_val() const {
    return maxVal;
}
