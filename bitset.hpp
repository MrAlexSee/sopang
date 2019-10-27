#ifndef BITSET_HPP
#define BITSET_HPP

#include <cstdint>
#include <cstring>
#include <set>

namespace sopang
{

template<int N>
class BitSet
{
public:
    BitSet(const std::initializer_list<int> &other);

    std::set<int> toSet() const;

    bool operator==(const BitSet &other) const;
    bool operator==(const std::set<int> &other) const;

    BitSet<N> operator&(const BitSet<N> &other) const;
    BitSet<N> operator|=(const BitSet<N> &other);

    int count() const;
    int empty() const;

    bool test(int n) const;

    void set();
    void set(int n);

    void reset();
    void reset(int n);

private:
    static constexpr uint64_t allOnes = ~(0x0ULL);
    static constexpr uint64_t allZeros = 0x0ULL;

    static constexpr int bufferSize = (N + 1) / 64;
    static constexpr int bufferSizeBytes = (N + 1) / 8;

    uint64_t buffer[bufferSize];
};

template<int N>
BitSet<N>::BitSet(const std::initializer_list<int> &list)
{
    for (const int n : list)
    {
        set(n);
    }
}

template <int N>
std::set<int> BitSet<N>::toSet() const
{
    std::set<int> ret;

    for (int i = 0; i < bufferSize; ++i)
    {
        const int offset = i * 64;

        for (int b = 0; b < 64; ++b)
        {
            if (buffer[i] & (0x1ULL << b))
            {
                ret.insert(offset + b);
            }
        }
    }

    return ret;
}

template <int N>
bool BitSet<N>::operator==(const BitSet &other) const
{
    return __builtin_memcmp(this->buffer, other.buffer, bufferSizeBytes) == 0;
}

template <int N>
bool BitSet<N>::operator==(const std::set<int> &other) const
{
    return this->toSet() == other;
}

template <int N>
BitSet<N> BitSet<N>::operator&(const BitSet<N> &other) const
{
    BitSet<N> ret;

    for (int i = 0; i < bufferSize; ++i)
    {
        ret.buffer[i] = this->buffer[i] & other.buffer[i];
    }

    return ret;
}

template <int N>
BitSet<N> BitSet<N>::operator|=(const BitSet<N> &other)
{
    for (int i = 0; i < bufferSize; ++i)
    {
        this->buffer[i] |= other.buffer[i];
    }

    return *this;
}

template <int N>
int BitSet<N>::count() const
{
    int ret = 0;

    for (int i = 0; i < bufferSize; ++i)
    {
        ret += __builtin_popcountll(buffer[i]);
    }

    return ret;
}

template <int N>
int BitSet<N>::empty() const
{
    for (int i = 0; i < bufferSize; ++i)
    {
        if (__builtin_popcountll(buffer[i]))
            return false;
    }

    return true;
}

namespace
{

// https://stackoverflow.com/questions/33333363/built-in-mod-vs-custom-mod-function-improve-the-performance-of-modulus-op/33333636
inline int modulo(const int input, const int ceil)
{
    return input >= ceil ? input % ceil : input;
}

} // namespace

template <int N>
bool BitSet<N>::test(int n) const
{
    return (buffer[n / 64] & (0x1ULL << (modulo(n, 64))));
}

template <int N>
void BitSet<N>::set()
{
    for (int i = 0; i < bufferSize; ++i)
    {
        buffer[i] = allOnes;
    }
}

template <int N>
void BitSet<N>::set(int n)
{
    buffer[n / 64] |= (0x1ULL << (modulo(n, 64)));
}

template <int N>
void BitSet<N>::reset()
{
    for (int i = 0; i < bufferSize; ++i)
    {
        buffer[i] = allZeros;
    }
}

template <int N>
void BitSet<N>::reset(int n)
{
    buffer[n / 64] &= (~(0x1ULL << (modulo(n, 64))));
}

} // namespace sopang

#endif // BITSET_HPP