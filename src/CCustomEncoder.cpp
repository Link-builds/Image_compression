#include "CCustomEncoder.h"

CustomEncoder::CustomEncoder(const Image& original) :
    m_width(original.width()), m_height(original.height()), m_channels(original.channels()), m_data(original.data())
{

}

void CustomEncoder::write_header(std::ofstream &out, uint32_t width, uint32_t height, uint8_t channels, uint8_t blockSize)
{
    out.write(reinterpret_cast<const char*>(&width), sizeof(width));
    out.write(reinterpret_cast<const char*>(&height), sizeof(height));
    out.write(reinterpret_cast<const char*>(&channels), sizeof(channels));
    out.write(reinterpret_cast<const char*>(&blockSize), sizeof(blockSize));
}

void CustomEncoder::write_selected_filters(std::ofstream &out, const std::vector<int> &selectedFilters)
{
    for (const auto &f : selectedFilters)
    {
        uint8_t index = static_cast<uint8_t>(f);
        out.write(reinterpret_cast<const char *>(&index), sizeof(index));
    }
}

void CustomEncoder::write_canon_lenghts(std::ofstream &out, const std::map<int, std::set<byte_t>> &canonLens)
{
    uint8_t size = static_cast<uint8_t>(canonLens.size());
    out.write(reinterpret_cast<const char *>(&size), sizeof(size));

    for (const auto &i : canonLens)
    {
        uint8_t len = static_cast<uint8_t>(i.first);
        uint16_t numSymbols = static_cast<uint16_t>(i.second.size());

        out.write(reinterpret_cast<const char *>(&len), sizeof(len));
        out.write(reinterpret_cast<const char *>(&numSymbols), sizeof(numSymbols));

        for (const auto &s : i.second)
            out.write(reinterpret_cast<const char *>(&s), sizeof(s));
    }
}

void CustomEncoder::write_bit(bool bit, uint8_t &curr ,int &bitCount, std::vector<uint8_t> &buffer)
{
    curr = (curr << 1) | (bit ? 1 : 0);
    bitCount++;

    if (bitCount == 8)
    {
        buffer.push_back(curr);
        curr = 0;
        bitCount = 0;
    }
}

void CustomEncoder::write_encoded_data(std::ofstream &out, const std::vector<int> &selectedFilters, const HuffmanCanon &dataEncoder)
{
    write_selected_filters(out, selectedFilters);
    write_canon_lenghts(out, dataEncoder.canonLens());

    std::vector<uint8_t> buffer;
    uint8_t curr = 0;
    int bitCount = 0;

    for (auto c : dataEncoder.encodedData())
        write_bit(c == '1', curr, bitCount, buffer);

    while (bitCount != 0 && bitCount < 8)
        write_bit(false, curr, bitCount, buffer);

    uint32_t dataLen = static_cast<uint32_t>(dataEncoder.encodedData().size());
    out.write(reinterpret_cast<const char *>(&dataLen), sizeof(dataLen));

    out.write(reinterpret_cast<const char *>(buffer.data()), buffer.size());
}

void CustomEncoder::encode_and_save(const std::string &outName)
{
    std::ofstream out(outName, std::ios::out | std::ios::binary);
    if (!out) throw std::runtime_error("ERR: Failed to save image (" + outName + ')');

    std::vector<int> selectedFilters = blockFilters::select_filters(m_width, m_height, m_channels, m_data, BLOCK_SIZE);
    std::vector<byte_t> filteredData = blockFilters::apply_filters(m_width, m_height, m_channels, m_data, BLOCK_SIZE, selectedFilters);

    auto dataEncoder = HuffmanCanon::as_encoder(filteredData); // calls incode in constructor

    uint32_t w = static_cast<uint32_t>(m_width);
    uint32_t h = static_cast<uint32_t>(m_height);
    uint8_t c = static_cast<uint8_t>(m_channels);
    uint8_t b = static_cast<uint8_t>(BLOCK_SIZE);

    write_header(out, w, h, c, b);
    write_encoded_data(out, selectedFilters, dataEncoder);
}