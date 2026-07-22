#include "filters.h"

int blockFilters::none(int left, int up, int upLeft)
{
    return 0;
}

int blockFilters::left(int left, int up, int upLeft)
{
    return left;
}

int blockFilters::up(int left, int up, int upLeft)
{
    return up;
}

int blockFilters::average(int left, int up, int upLeft)
{
    return (left + up) / 2;
}

int blockFilters::paeth(int left, int up, int upLeft)
{
    int p = left + up - upLeft;
    int pLeft = std::abs(p - left);
    int pUp = std::abs(p - up);
    int pUL = std::abs(p - upLeft);

    if (pLeft <= pUp && pLeft <= pUL)
        return left;
    else if (pUp <= pUL)
        return up;
    else
        return upLeft;
}

int blockFilters::loco(int left, int up, int upLeft)
{
    if (upLeft >= std::max(left, up))
        return std::min(left, up);

    if (upLeft <= std::min(left, up))
        return std::max(left, up);

    return left + up - upLeft;
}

std::vector<int> select_filters(int width, int height, int channels ,const unsigned char *data ,int blockSize)
{
    int blocksPerRow = (width + blockSize - 1) / blockSize;
    int blocksPerCol = (height + blockSize - 1) / blockSize;

    std::vector<int> selectedFilters(blocksPerRow*blocksPerCol);

    for (int h = 0; h < height; h += blockSize)
        for (int w = 0; w < width; w += blockSize)
        {
            int bestFilter = 0;
            long long bestScore = LLONG_MAX;

            int blockHeight = std::min(blockSize, height - h);
            int blockWidth = std::min(blockSize, width - w);

            for (size_t f = 0; f < blockFilters::selection.size(); f++)
            {
                long long currScore = 0;

                for (int i = 0; i < blockHeight; i++)
                    for (int j = 0; j < blockWidth; j++)
                    {
                        int y = h + i;
                        int x = w + j;

                        int index = (y*width + x)*channels;

                        for (int k = 0; k < channels; k++)
                        {
                            int pos = index + k;
                            int l = pos - channels;
                            int u = pos - width*channels;
                            int uL = pos - width*channels - channels;

                            int left = (x > 0) ? data[l] : 0;
                            int up = (y > 0) ? data[u] : 0;
                            int upLeft = (x > 0 && y > 0) ? data[uL] : 0;

                            int raw = data[pos];
                            int predicted = blockFilters::selection[f](left, up, upLeft);

                            currScore += std::abs(raw - predicted);
                        }
                    }

                if (currScore < bestScore)
                {
                    bestScore = currScore;
                    bestFilter = static_cast<int>(f);
                }
            }

            int blockRow = h / blockSize;
            int blockCol = w / blockSize;
            int blockIndex = blockRow*blocksPerRow + blockCol;

            selectedFilters[blockIndex] = bestFilter;
        }

    return selectedFilters;
}