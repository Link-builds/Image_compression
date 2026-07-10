#include "utils.h"

namespace fs = std::filesystem;

constexpr int MAX_PIXEL_VAL = 255;
constexpr int WINDOW_SIZE = 8;
constexpr int TIMER_CYCLES = 7;

void utils::prepare_dataset(const std::string &srcDir, const std::string &destDir)
{
    fs::create_directories(destDir);

    for (const auto &i : fs::directory_iterator(srcDir))
    {
        if (!i.is_regular_file()) continue;
        if (i.path().extension() != ".png") continue;

        try
        {
            Image img(i.path().string());
            
            std::string outName = destDir + '/' + i.path().stem().string();
            img.save_as(outName, Format::BMP);
        }
        catch(const std::exception& e)
        {
            std::cerr << "Failed to convert " << i.path().filename() << ": " << e.what() << '\n';
        }        
    }
}

double utils::get_median(std::vector<double> &values)
{
    if (values.empty()) return 0.0;

    std::sort(values.begin(), values.end());
    size_t n = values.size();

    if (!(n % 2))
        return (values[(n-1) / 2] + values[n/2]) / 2.0;

    return values[n/2];
}

double utils::get_mean(const std::vector<double> &values)
{
    double sum = 0.0;
    int n = values.size();

    for (int i = 0; i < n; i++)
        sum += values[i];

    return sum / n;
}

double utils::get_variance(const std::vector<double> &values, double mean)
{
    double sumProd = 0.0;
    int n = values.size();

    for (int i = 0; i < n; i++)
        sumProd += (values[i] - mean)*(values[i] - mean);

    return sumProd / n;
}

double utils::get_covariance(const std::vector<double> &xValues, const std::vector<double> &yValues, double xMean, double yMean)
{
    double sumProd = 0.0;
    int n = xValues.size();

    for (int i = 0; i < n; i++)
        sumProd += (xValues[i] - xMean) * (yValues[i] - yMean);

    return sumProd / n;
}

double utils::get_psnr(const Image &original, const Image &reconstructed)
{
    int m = original.height();
    int n = original.width();
    int c = original.channels();
    long long sum = 0;

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < c; k++)
            {
                int index = (i*n + j) * c + k;
                int diff = static_cast<int>(original.data()[index]) - static_cast<int>(reconstructed.data()[index]);
                sum += diff*diff;
            }

    double mse = static_cast<double>(sum) / (m*n*c);
    if (mse == 0.0) return std::numeric_limits<double>::infinity();

    double psnr = 20*log10(MAX_PIXEL_VAL) - 10*log10(mse);

    return psnr;
}

std::vector<double> utils::out_channel(const Image &img, int channel)
{
    int m = img.height();
    int n = img.width();
    int c = img.channels();

    std::vector<double> chOut;
    chOut.reserve(n*m);

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
        {
            int index = (i*n + j) * c + channel;
            chOut.push_back(img.data()[index]);
        }

    return chOut;
}

double utils::get_channel_ssim(const std::vector<double> &xChannel, const std::vector<double> &yChannel, int width, int height)
{

    double L = MAX_PIXEL_VAL;
    double C1 = (0.01*L)*(0.01*L);
    double C2 = (0.03*L)*(0.03*L);

    // SSIM(x, y) = (2*meanX*meanY + C1) * (2*covXY + C2) / (meanX*meanX + meanY*meanY + C1) * (varX + varY + C2)

    double sumSSIM = 0.0;
    int windowCount = 0;

    for (int h = 0; h < height; h += WINDOW_SIZE)
        for (int w = 0; w < width; w += WINDOW_SIZE)
        {
            int winHeight = std::min(WINDOW_SIZE, height - h);
            int winWidth = std::min(WINDOW_SIZE, width - w);

            std::vector<double> winX, winY;

            for (int i = 0; i < winHeight; i++)
                for (int j = 0; j < winWidth; j++)
                {
                    int index = (h + i)*width + (w + j);
                    winX.push_back(xChannel[index]);
                    winY.push_back(yChannel[index]);
                }

            double meanX = get_mean(winX);
            double meanY = get_mean(winY);
            double varX = get_variance(winX, meanX);
            double varY = get_variance(winY, meanY);
            double covXY = get_covariance(winX, winY, meanX, meanY);

            double part1 = (2*meanX*meanY + C1) * (2*covXY + C2);
            double part2 = (meanX*meanX + meanY*meanY + C1) * (varX + varY + C2);

            sumSSIM += part1 / part2;
            windowCount++;
        }

    return sumSSIM / windowCount;
}

double utils::get_ssim(const Image &original, const Image &reconstructed)
{
    double totalSSIM = 0.0;
    int channels = std::min(original.channels(), 3);

    for (int ch = 0; ch < channels; ch++)
    {
        std::vector<double> xChannel = out_channel(original, ch);
        std::vector<double> yChannel = out_channel(reconstructed, ch);

        totalSSIM += get_channel_ssim(xChannel, yChannel, original.width(), original.height());
    }

    return totalSSIM / channels;
}

utils::EvalResult utils::run_case(
    const Image &original,
    const std::string &outName,
    Format format,
    std::optional<int> quality,
    const std::string &ext,
    size_t ogSize,
    const std::string &codec)
{
    std::vector<double> encTimes;
    encTimes.reserve(TIMER_CYCLES);
    
    std::vector<double> decTimes;
    decTimes.reserve(TIMER_CYCLES);

    for (int i = 0; i < TIMER_CYCLES; i++)
    {
        auto t0 = std::chrono::steady_clock::now();
        original.save_as(outName, format, quality);
        auto t1 = std::chrono::steady_clock::now();

        double encTime = std::chrono::duration<double, std::milli>(t1 - t0).count();
        encTimes.push_back(encTime);
    }

    for (int i = 0; i < TIMER_CYCLES; i++)
    {
        auto t0 = std::chrono::steady_clock::now();
        Image decoded(outName + ext);
        auto t1 = std::chrono::steady_clock::now();

        double decTime = std::chrono::duration<double, std::milli>(t1 - t0).count();
        decTimes.push_back(decTime);
    }

    Image decoded(outName + ext);
    
    EvalResult res;
    res.fileName = outName;
    res.codec = codec;
    res.quality = quality.value_or(-1);
    res.originalSize = ogSize;
    res.compressedSize = fs::file_size(outName + ext);
    res.compressionRatio = static_cast<double>(ogSize) / static_cast<double>(res.compressedSize);
    res.encodeTime = get_median(encTimes);
    res.decodeTime = get_median(decTimes);
    res.psnr = get_psnr(original, decoded);
    res.ssim = get_ssim(original, decoded);
    
    return res;
}

void utils::generate_csv(const std::vector<EvalResult> &results, const std::string &filePath)
{
    std::ofstream csv(filePath);

    csv << std::fixed << std::setprecision(6);
    csv << "fileName,codec,quality,originalSize,compressedSize,compressionRatio,encodeTime,decodeTime,psnr,ssim\n";

    for (const auto &x : results)
    {
        csv << x.fileName << ','
            << x.codec << ','
            << x.quality << ','
            << x.originalSize << ','
            << x.compressedSize << ','
            << x.compressionRatio << ','
            << x.encodeTime << ','
            << x.decodeTime << ','
            << x.psnr << ','
            << x.ssim << '\n';
    }
}

void utils::evaluate_metrics(const std::string &srcDir)
{
    std::vector<EvalResult> results;
    fs::create_directories("output/BMP");
    fs::create_directories("output/PNG");
    fs::create_directories("output/JPG");

    for (const auto &i : fs::directory_iterator(srcDir))
    {
        Image img(i.path().string());
        size_t ogSize = fs::file_size(i.path());

        std::string outName = "output/BMP/" + i.path().stem().string();
        results.emplace_back(run_case(img, outName, Format::BMP, std::nullopt,".bmp", ogSize, "BMP"));

        outName = "output/PNG/" + i.path().stem().string();
        results.emplace_back(run_case(img, outName, Format::PNG, std::nullopt,".png", ogSize, "PNG"));

        for (int quality : {1, 10, 30, 50, 70, 90})
        {
            outName = "output/JPG/" + i.path().stem().string() + "_q" + std::to_string(quality);
            results.emplace_back(run_case(img, outName, Format::JPG, quality,".jpg", ogSize, "JPG"));
        }
    }

    generate_csv(results, "output/results.csv");
}