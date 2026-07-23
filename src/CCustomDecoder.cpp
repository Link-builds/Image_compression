#include "CCustomDecoder.h"

void CustomDecoder::read_header(std::ifstream &in)
{
    uint32_t width;
    uint32_t height;
    uint8_t channels;
    uint8_t blockSize;

    in.read(reinterpret_cast<char*>(&width), sizeof(width));
    in.read(reinterpret_cast<char*>(&height), sizeof(height));
    in.read(reinterpret_cast<char*>(&channels), sizeof(channels));
    in.read(reinterpret_cast<char*>(&blockSize), sizeof(blockSize));

    m_width = static_cast<int>(width);
    m_height = static_cast<int>(height);
    m_channels = static_cast<int>(channels);
    m_blockSize = static_cast<int>(blockSize);
}

std::vector<int> CustomDecoder::read_selected_filters(std::ifstream &in)
{
    int blocksPerRow = (m_width + m_blockSize - 1) / m_blockSize;
    int blocksPerCol = (m_height + m_blockSize - 1) / m_blockSize;
    size_t blockCount = static_cast<size_t>(blocksPerRow) * blocksPerCol;

    std::vector<int> selectedFilters(blockCount);

    for (size_t i = 0; i < blockCount; i++)
    {
        uint8_t f;
        in.read(reinterpret_cast<char *>(&f), sizeof(f));

        selectedFilters[i] = static_cast<int>(f);
    }

    return selectedFilters;
}

std::map<int, std::set<byte_t>> CustomDecoder::read_canon_lenghts(std::ifstream &in)
{
    std::map<int, std::set<byte_t>> canonLens;

    uint8_t size;
    in.read(reinterpret_cast<char *>(&size), sizeof(size));

    for (uint8_t i = 0; i < size; i++)
    {
        uint8_t len;
        uint16_t numSymbols;

        in.read(reinterpret_cast<char *>(&len), sizeof(len));
        in.read(reinterpret_cast<char *>(&numSymbols), sizeof(numSymbols));
        
        std::set<byte_t> symbols;

        for (uint16_t j = 0; j < numSymbols; j++)
        {
            byte_t s;
            in.read(reinterpret_cast<char *>(&s), sizeof(s));
            symbols.insert(s);
        }

        canonLens[len] = symbols;
    }

    return canonLens;
}

bool CustomDecoder::read_bit(const std::vector<uint8_t> &buffer, size_t &byteIndex, int &bitIndex)
{
    bool bit = (buffer[byteIndex] >> (7 - bitIndex)) & 1;

    bitIndex++;

    if (bitIndex == 8)
    {
        bitIndex = 0;
        byteIndex++;
    }

    return bit;
}

std::string CustomDecoder::read_encoded_data(std::ifstream &in)
{
    uint32_t dataLen;
    in.read(reinterpret_cast<char *>(&dataLen), sizeof(dataLen));

    size_t byteCount = (static_cast<size_t>(dataLen) + 7) / 8;
    std::vector<uint8_t> buffer(byteCount);
    in.read(reinterpret_cast<char*>(buffer.data()), byteCount);

    std::string res;
    size_t len = static_cast<size_t>(dataLen);
    res.reserve(len);

    size_t byteIndex = 0;
    int bitIndex = 0;

    for (size_t i = 0; i < len; i++)
        res += read_bit(buffer, byteIndex, bitIndex) ? '1' : '0';

    return res;
}

void CustomDecoder::decode_and_load()
{
    std::ifstream in(m_dataPath, std::ios::in | std::ios::binary);
    if (!in) throw std::runtime_error("ERR: Failed to load image (" + m_dataPath + ')');

    read_header(in);
    std::vector<int> selectedFilters = read_selected_filters(in);

    std::map<int, std::set<byte_t>> canonLens = read_canon_lenghts(in);
    std::string encodedData = read_encoded_data(in);

    auto dataDecoder = HuffmanCanon::as_decoder(canonLens, encodedData);

    std::string decoded = dataDecoder.decode();

    m_data = blockFilters::remove_filters(m_width, m_height, m_channels, decoded, m_blockSize, selectedFilters);
}