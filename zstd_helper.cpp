#include "zstd_helper.hpp"

#include <cstring>

#include <zstd.h>

using namespace std;

string decompressZstd(const string &compressed, size_t bufferSize)
{
    string ret;
    ZSTD_DCtx *zstdContext = ZSTD_createDCtx();

    char *inBuffer = new char[bufferSize];
    char *outBuffer = new char[bufferSize];

    size_t inputSize = bufferSize;

    for (size_t i = 0; i < compressed.size(); i += bufferSize)
    {
        if (i + bufferSize > compressed.size())
        {
            bufferSize = compressed.size() - i;
        }

        memcpy(inBuffer, compressed.c_str() + i, inputSize);
        ZSTD_inBuffer input = { inBuffer, inputSize, 0 };

        while (input.pos < input.size)
        {
            ZSTD_outBuffer output = { outBuffer, bufferSize, 0 };
            const size_t zstdCode = ZSTD_decompressStream(zstdContext, &output, &input);

            if (ZSTD_isError(zstdCode))
            {
                break;
            }

            ret += string(outBuffer, output.pos);
        }
    }

    return ret;
}
