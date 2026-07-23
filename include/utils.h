#pragma once

#include <filesystem>
#include <chrono>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <numeric>

#include "CImage.h"

namespace utils
{
    struct EvalResult
    {
        std::string fileName;
        std::string codec;
        int quality;
        size_t originalSize;
        size_t compressedSize;
        double compressionRatio;
        double encodeTime;
        double decodeTime;
        double psnr;
        double ssim;
    };

    void prepare_dataset(const std::string &srcDir, const std::string &destDir);

    double get_median(std::vector<double> values);
    double get_mean(const std::vector<double> &values);
    double get_variance(const std::vector<double> &values, double mean);
    double get_covariance(const std::vector<double> &xValues, const std::vector<double> &yValues, double xMean, double yMean);

    double get_psnr(const Image &original, const Image &reconstructed);

    std::vector<double> out_channel(const Image &img, int channel);
    double get_channel_ssim(const std::vector<double> &xChannel, const std::vector<double> &yChannel, int width, int height);
    double get_ssim(const Image &original, const Image &reconstructed);

    EvalResult run_case(const Image &original, const std::string &outName, Format format, std::optional<int> quality,
                        const std::string &ext, size_t ogSize, const std::string &codec);

    void evaluate_metrics(const std::string &srcDir);
    void generate_csv(const std::vector<EvalResult> &results, const std::string &filePath);

    bool has_extension(const std::string& path, const std::string& ext);
}