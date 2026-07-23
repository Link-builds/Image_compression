#include "CApp.h"

namespace fs = std::filesystem;

void App::run()
{
    // BMP control group dataset
    if (!fs::exists("resource/DatasetBMP") || fs::is_empty("resource/DatasetBMP"))
        utils::prepare_dataset("resource/Kodak", "resource/DatasetBMP");

    utils::evaluate_metrics("resource/DatasetBMP");

    fs::create_directories("output/POLinBMP");

    for (const auto &i : fs::directory_iterator("output/POL"))
    {
        std::string dataPath = i.path().string();

        Image img(dataPath);
        
        std::string outName = "output/POLinBMP/" + i.path().stem().string();

        img.save_as(outName, Format::BMP);
    }
}