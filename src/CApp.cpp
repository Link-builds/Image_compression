#include "CApp.h"

namespace fs = std::filesystem;

void App::run()
{
    // BMP control group dataset
    if (!fs::exists("resource/DatasetBMP") || fs::is_empty("resource/DatasetBMP"))
        utils::prepare_dataset("resource/Kodak", "resource/DatasetBMP");

    utils::evaluate_metrics("resource/DatasetBMP");
}