#ifndef SOPANG_WHITEBOX_HPP
#define SOPANG_WHITEBOX_HPP

#ifndef SOPANG_WHITEBOX
#define SOPANG_WHITEBOX \
    friend class SopangWhitebox;
#endif

#include "../sopang.hpp"

namespace inverted_basilisk
{

class SopangWhitebox
{
public:
    SopangWhitebox() = delete;

    inline static void fillPatternMaskBuffer(Sopang &sopang, const string &arg1, const string &arg2)
    {
        sopang.fillPatternMaskBuffer(arg1, arg2);
    }
    inline static void fillPatternMaskBufferApprox(Sopang &sopang, const string &arg1, const string &arg2)
    {
        sopang.fillPatternMaskBufferApprox(arg1, arg2);
    }

    inline static const uint64_t *getMaskBuffer(const Sopang &sopang)
    {
        return sopang.maskBuffer;
    }

    inline static unsigned getWordSize(const Sopang &sopang) 
    {
        return sopang.wordSize;
    }
    inline static unsigned getSACounterSize(const Sopang &sopang) 
    {
        return sopang.saCounterSize;
    }
};

}

#endif // SOPANG_WHITEBOX_HPP
