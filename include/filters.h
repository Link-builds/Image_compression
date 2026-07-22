#include <iostream>
#include <cstdlib>
#include <array>
#include <vector>
#include <climits>

namespace blockFilters
{
    using filterFun = int (*)(int left, int up, int upLeft);

    int none(int left, int up, int upLeft);

    int left(int left, int up, int upLeft);

    int up(int left, int up, int upLeft);

    int average(int left, int up, int upLeft);

    int paeth(int left, int up, int upLeft);

    int loco(int left, int up, int upLeft);

    constexpr std::array<filterFun, 6> selection = {none, left, up, average, paeth, loco};

    std::vector<int> select_filters(int width, int height, int channels ,const unsigned char *data ,int blockSize);
}