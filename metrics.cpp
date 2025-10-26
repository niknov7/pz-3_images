#include "metrics.h"
#include <cmath>
#include <limits>
#include <stdexcept>

template<typename Func>
double Metrics::pixel_accumulate(const PGMImage &img1, const PGMImage &img2, Func func) {
    auto [w1, h1] = img1.get_size();
    auto [w2, h2] = img2.get_size();

    if (w1 != w2 || h1 != h2)
        throw std::invalid_argument("Images must have the same dimensions");

    double sum = 0.0;
    for (int i = 0; i < h1; ++i) {
        for (int j = 0; j < w1; ++j) {
            sum += func(static_cast<int>(img1(i, j)) - static_cast<int>(img2(i, j)));
        }
    }

    return sum / (w1 * h1);
}

// MSE (Mean Squared Error)
// Среднеквадратичная ошибка показывает среднее квадратичное отклонение пикселей
// обработанного изображения от исходного. Практически это удобно, чтобы видеть,
// насколько сильно алгоритм «ломает» картинку на отдельных участках: большие ошибки
// сразу сильно увеличивают MSE.
double Metrics::mse(const PGMImage &original, const PGMImage &processed) {
    return pixel_accumulate(original, processed, [](const int diff) { return diff * diff; });
}

// MAE (Mean Absolute Error)
// Средняя абсолютная ошибка усредняет модуль разницы пикселей.
// Показывает среднее отклонение яркости между оригиналом и обработанным изображением.
// MAE менее чувствительна к редким большим артефактам по сравнению с MSE.
double Metrics::mae(const PGMImage &original, const PGMImage &processed) {
    return pixel_accumulate(original, processed, [](const int diff) { return std::abs(diff); });
}

// PSNR (Peak Signal-to-Noise Ratio)
// Пиковое отношение сигнал/шум основано на MSE, но представлено в децибелах.
// Высокая PSNR означает, что визуально шум почти не заметен,
// и обработка не ухудшила качество изображения.
double Metrics::psnr(const PGMImage &original, const PGMImage &processed) {
    double mse_val = mse(original, processed);
    if (mse_val == 0.0)
        return std::numeric_limits<double>::infinity();

    double maxVal = static_cast<double>(original.get_max_val());
    return 10.0 * std::log10((maxVal * maxVal) / mse_val);
}

// UQI (Universal Quality Index)
// Универсальный индекс качества учитывает не только разницу пикселей,
// но и корреляцию, контраст и яркость.
// UQI ближе к тому, как человек воспринимает качество изображения.
// Значение 1 означает полное совпадение с оригиналом, значения меньше 1 показывают ухудшение.
double Metrics::uqi(const PGMImage &original, const PGMImage &processed) {
    auto [w, h] = original.get_size();
    auto [w2, h2] = processed.get_size();
    if (w != w2 || h != h2)
        throw std::invalid_argument("Images must have the same dimensions for UQI.");

    double meanX = 0.0, meanY = 0.0;
    double varX = 0.0, varY = 0.0;
    double covXY = 0.0;
    int n = w * h;

    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j) {
            meanX += original(i, j);
            meanY += processed(i, j);
        }
    meanX /= n;
    meanY /= n;

    for (int i = 0; i < h; ++i)
        for (int j = 0; j < w; ++j) {
            double dx = static_cast<double>(original(i, j)) - meanX;
            double dy = static_cast<double>(processed(i, j)) - meanY;
            varX += dx * dx;
            varY += dy * dy;
            covXY += dx * dy;
        }

    if ((varX + varY) == 0.0 || (meanX * meanX + meanY * meanY) == 0.0)
        return 1.0;

    return (4.0 * covXY * meanX * meanY) / ((varX + varY) * (meanX * meanX + meanY * meanY));
}
