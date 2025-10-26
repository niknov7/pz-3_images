#ifndef METRICS_H
#define METRICS_H

#include "PGMImage.h"

class Metrics {
public:
    static double mse(const PGMImage &original, const PGMImage &processed);
    static double mae(const PGMImage &original, const PGMImage &processed);
    static double psnr(const PGMImage &original, const PGMImage &processed);
    static double uqi(const PGMImage &original, const PGMImage &processed);

private:
    template<typename Func>
    static double pixel_accumulate(const PGMImage &img1, const PGMImage &img2, Func func);
};

#endif // METRICS_H
