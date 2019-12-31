#include "zstd_helper.hpp"

#include <iostream>

#include <zstd.h>

using namespace std;

namespace zstd
{

string decompress(const string &compressed)
{
    const unsigned long long frameSize = ZSTD_getFrameContentSize(compressed.c_str(), compressed.size());

    if (frameSize == ZSTD_CONTENTSIZE_ERROR or frameSize == ZSTD_CONTENTSIZE_UNKNOWN)
    {
        cerr << "Zstd decompression failed, missing frame content size" << endl;
        return "";
    }

    char *dstBuffer = new char[frameSize];
    const size_t decompressedSize = ZSTD_decompress(dstBuffer, frameSize, compressed.c_str(), compressed.size());

    if (ZSTD_isError(decompressedSize))
    {
        cerr << "Zstd compression failed" << endl;
        return "";
    }

    const string ret(dstBuffer, dstBuffer + decompressedSize);

    delete[] dstBuffer;
    return ret;
}

}
