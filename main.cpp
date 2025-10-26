#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

#include "CSVWriter.h"
#include "metrics.h"
#include "PGMImage.h"


int main() {
    std::filesystem::path originalDir = "original";

    std::filesystem::path noizeLowDir = "noizeLow";
    std::filesystem::path noizeMediumDir = "noizeMedium";
    std::filesystem::path noizeHighDir = "noizeHigh";

    std::filesystem::path cleanedLowDir = "cleanedLow";
    std::filesystem::path cleanedMediumDir = "cleanedMedium";
    std::filesystem::path cleanedHighDir = "cleanedHigh";

    for (auto &dir: {
             originalDir, noizeLowDir, noizeMediumDir, noizeHighDir,
             cleanedLowDir, cleanedMediumDir, cleanedHighDir
         }) {
        if (!std::filesystem::exists(dir)) std::filesystem::create_directory(dir);
    }

    bool hasFiles = false;

    CSVWriter csv("metrics.csv", {
                      "Filename", "Level", "PSNR", "MSE", "MAE", "UQI"
                  });

    for (const auto &entry: std::filesystem::directory_iterator(originalDir)) {
        if (entry.path().extension() == ".pgm") {
            hasFiles = true;
            std::string filename = entry.path().filename().string();

            PGMImage image(entry.path().string());

            PGMImage imgLow = image;
            imgLow.add_uniform_noise(15.0);
            imgLow.add_gaussian_noise(7.0);
            imgLow.add_salt_and_pepper_noise(0.02);
            imgLow.save((noizeLowDir / filename).string());

            PGMImage cleanLow = imgLow;
            cleanLow.median_filter(3);
            cleanLow.exponential_moving_average(0.3);
            cleanLow.save((cleanedLowDir / filename).string());

            PGMImage imgMedium = image;
            imgMedium.add_uniform_noise(25.0);
            imgMedium.add_gaussian_noise(12.0);
            imgMedium.add_salt_and_pepper_noise(0.03);
            imgMedium.save((noizeMediumDir / filename).string());

            PGMImage cleanMedium = imgMedium;
            cleanMedium.median_filter(3);
            cleanMedium.moving_average_filter(5);
            cleanMedium.exponential_moving_average(0.3);
            cleanMedium.save((cleanedMediumDir / filename).string());

            PGMImage imgHigh = image;
            imgHigh.add_uniform_noise(40.0);
            imgHigh.add_gaussian_noise(20.0);
            imgHigh.add_salt_and_pepper_noise(0.05);
            imgHigh.save((noizeHighDir / filename).string());

            PGMImage cleanHigh = imgHigh;
            cleanHigh.median_filter(5);
            cleanHigh.moving_average_filter(3);
            cleanHigh.exponential_moving_average(0.2);
            cleanHigh.save((cleanedHighDir / filename).string());

            std::vector<std::string> row;

            row = {
                filename, "Low",
                CSVWriter::toCsvString(Metrics::psnr(image, cleanLow)),
                CSVWriter::toCsvString(Metrics::mse(image, cleanLow)),
                CSVWriter::toCsvString(Metrics::mae(image, cleanLow)),
                CSVWriter::toCsvString(Metrics::uqi(image, cleanLow))
            };
            csv.addRow(row);

            row = {
                filename, "Medium",
                CSVWriter::toCsvString(Metrics::psnr(image, cleanMedium)),
                CSVWriter::toCsvString(Metrics::mse(image, cleanMedium)),
                CSVWriter::toCsvString(Metrics::mae(image, cleanMedium)),
                CSVWriter::toCsvString(Metrics::uqi(image, cleanMedium))
            };
            csv.addRow(row);

            row = {
                filename, "High",
                CSVWriter::toCsvString(Metrics::psnr(image, cleanHigh)),
                CSVWriter::toCsvString(Metrics::mse(image, cleanHigh)),
                CSVWriter::toCsvString(Metrics::mae(image, cleanHigh)),
                CSVWriter::toCsvString(Metrics::uqi(image, cleanHigh))
            };
            csv.addRow(row);
        }
    }

    if (!hasFiles) {
        std::cout << "The folder 'original' is empty or no .pgm files were found." << std::endl;
        std::cout << "Please place your PGM images into the 'original' folder and run the program again." << std::endl;
        return 1;
    }

    csv.save();
    std::cout << "Metrics saved to metrics.csv" << std::endl;

    std::cout << "Press any key to exit..." << std::endl;
    std::cin.get();
    return 0;
}
