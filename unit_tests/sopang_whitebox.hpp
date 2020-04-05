#ifndef SOPANG_WHITEBOX_HPP
#define SOPANG_WHITEBOX_HPP

#ifndef SOPANG_WHITEBOX
#define SOPANG_WHITEBOX \
    friend class SopangWhitebox;
#endif

#include "../sopang.hpp"

namespace sopang
{

class SopangWhitebox
{
public:
    SopangWhitebox() = delete;

    inline static void fillPatternMaskBuffer(Sopang &sopang, const std::string &arg1)
    {
        sopang.fillPatternMaskBuffer(arg1);
    }

    inline static void fillPatternMaskBufferApprox(Sopang &sopang, const std::string &arg1)
    {
        sopang.fillPatternMaskBufferApprox(arg1);
    }

    inline static const uint64_t *getMaskBuffer(const Sopang &sopang)
    {
        return sopang.maskBuffer;
    }

    inline static int getMaxSourceCount(const Sopang &sopang)
    {
        return sopang.maxSourceCount;
    }

    inline static size_t getWordSize(const Sopang &sopang) 
    {
        return sopang.wordSize;
    }

    inline static size_t getSACounterSize(const Sopang &sopang) 
    {
        return sopang.saCounterSize;
    }
};

} // namespace sopang

#endif // SOPANG_WHITEBOX_HPP
