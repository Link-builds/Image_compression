#pragma once

#include <fstream>
#include <cstdint>

#include "CHuffmanCanon.h"
#include "filters.h"
#include "CImage.h"

constexpr int BLOCK_SIZE = 8;

class CustomEncoder
{
private:
    int m_width;
    int m_height;
    int m_channels;
    const byte_t *m_data;

    void write_header(std::ofstream &out, uint32_t width, uint32_t height, uint8_t channels, uint8_t blockSize);
    void write_selected_filters(std::ofstream &out, const std::vector<int> &selectedFilters);
    void write_canon_lenghts(std::ofstream &out, const std::map<int, std::set<byte_t>> &canonLens);
    void write_bit(bool bit, uint8_t &curr, int &bitCount, std::vector<uint8_t> &buffer);

    void write_encoded_data(std::ofstream &out, const std::vector<int> &selectedFilters, const HuffmanCanon &dataEncoder);

public:
    CustomEncoder(const Image &original);

    ~CustomEncoder() {};

    void encode_and_save(const std::string &outName);
};