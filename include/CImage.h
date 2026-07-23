#pragma once

#include <stb_image.h>
#include <stb_image_write.h>

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <optional>
#include <cstdlib>
#include <cstring>

enum class Format
{
    PNG,
    JPG,
    BMP,
    POL
};

class Image
{
private:
    int m_width;
    int m_height;
    int m_numChannel;
    const std::string m_dataPath;
    unsigned char *m_data;

public:
    Image(const std::string &dataPath);

    Image(const Image &) = delete;
    Image & operator = (const Image &) = delete;

    ~Image();

    const unsigned char *data() const { return m_data; };
    int width() const { return m_width; };
    int height() const { return m_height; };
    int channels() const { return m_numChannel; };

    void print_info() const;
    void save_as(const std::string &outName, Format format, std::optional<int> quality = std::nullopt) const;
}; 