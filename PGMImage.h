#ifndef PGMIMAGE_H
#define PGMIMAGE_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <limits>

class PGMImage {
private:
    int width;
    int height;
    unsigned short maxVal;
    std::vector<unsigned short> pixels;

public:
    PGMImage();

    explicit PGMImage(const std::string &filename);

    bool load(const std::string &filename);
    bool save(const std::string &filename) const;

    std::pair<int, int> get_size() const;
    unsigned short get_max_val() const;

    unsigned short& operator()(int row, int col);
    const unsigned short& operator()(int row, int col) const;

    void add_uniform_noise(double levelPercent);
    void add_salt_and_pepper_noise(double probability);
    void add_gaussian_noise(double sigmaPercent);

    void median_filter(int windowSize);
    void exponential_moving_average(double k);
    void moving_average_filter(int size);
};
#endif //PGMIMAGE_H
