#include "CImage.h"

Image::Image(const std::string &dataPath) : m_dataPath(dataPath)
{
    m_data = stbi_load(dataPath.c_str(), &m_width, &m_height, &m_numChannel, 0);

    if (!m_data) throw std::runtime_error("ERR: Failed to load image (" + m_dataPath + ')');
}

Image::~Image()
{
    stbi_image_free(m_data);
}

void Image::print_info() const
{
    std::cout << m_dataPath << ": " << m_width << 'x' << m_height << " (ch: " << m_numChannel << ')' << '\n'; 
}

void Image::save_as(const std::string &outName, Format format, std::optional<int> quality) const
{
    if (format == Format::PNG)
    {
        stbi_write_png((outName + ".png").c_str(), m_width, m_height, m_numChannel, m_data, m_width * m_numChannel);
    }
    else if (format == Format::JPG)
    {
        int q;
        
        if (quality.has_value())
        {
            q = quality.value();
        }
        else
        {
            std::cout << "You chose to save as .jpg. Please provide a quality value [1-100]: ";
            std::cin >> q;

            while( q < 1 || q > 100 )
            {
                std::cout << "Provided value out of scope, must be [1-100]: ";
                std::cin >> q;
            }
        }

        stbi_write_jpg((outName + ".jpg").c_str(), m_width, m_height, m_numChannel, m_data, q);
    }
    else if (format == Format::BMP)
    {
        stbi_write_bmp((outName + ".bmp").c_str(), m_width, m_height, m_numChannel, m_data);
    }
}