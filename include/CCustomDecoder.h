#pragma once

#include <fstream>
#include <cstdint>

#include "CHuffmanCanon.h"
#include "filters.h"
#include "CImage.h"

class CustomDecoder
{
private:
    int m_width;
    int m_height;
    int m_channels;
    int m_blockSize;
    const std::string m_dataPath;
    std::vector<byte_t> m_data;

    void read_header(std::ifstream &in);
    std::vector<int> read_selected_filters(std::ifstream &in);
    std::map<int, std::set<byte_t>> read_canon_lenghts(std::ifstream &in);
    bool read_bit(const std::vector<uint8_t> &buffer, size_t &byteIndex, int &bitIndex);

    std::string read_encoded_data(std::ifstream &in);

public:
    CustomDecoder(const std::string &dataPath) : m_dataPath(dataPath) {};

    ~CustomDecoder() {};

    void decode_and_load();

    int width() const { return m_width; };
    int height() const { return m_height; };
    int channels() const { return m_channels; };
    byte_t * data() { return m_data.data(); }; 
};