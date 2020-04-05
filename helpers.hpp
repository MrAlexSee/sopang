#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <algorithm>
#include <cmath>
#include <fstream>
#include <random>
#include <stdexcept>
#include <string>
#include <sstream>
#include <type_traits>

namespace sopang::helpers
{

/*
*** COLLECTIONS
*/

/** Calculates a median from [vec] and stores it in [median]. */
template<typename T>
void calcStatsMedian(const std::vector<T> &vec, T *median);

/*
*** FILES
*/

inline bool isFileReadable(const std::string &filePath);
inline std::string readFile(const std::string &filePath);

/** Appends [text] to file with [filePath] followed by an optional newline if [newline] is true. */
inline void dumpToFile(const std::string &text, const std::string &filePath, bool newline = false);
inline bool removeFile(const std::string &filePath);

/*
*** RANDOM
*/

/** Returns a random integer from range [start] to [end] (both inclusive) not equal to [excluded]. */
inline int randIntRangeExcluded(int start, int end, int excluded);

/*
*** STRINGS
*/

template<typename T>
std::string join(const std::vector<T> &vec, const std::string &delim);

inline void removeEmptyStrings(std::vector<std::string> &vec);

/** Returns a random string having [size] characters sampled uniformly from [alphabet]. */
inline std::string genRandomString(int size, const std::string &alphabet);
/** Returns a random alphanumeric string having [size] characters. */
inline std::string genRandomStringAlphNum(int size);

template<typename T>
void calcStatsMedian(const std::vector<T> &vec, T *median)
{
    if (vec.empty())
        return;

    std::vector<T> tmp = vec;

    std::sort(tmp.begin(), tmp.end());
    *median = tmp[tmp.size() / 2];
}

bool isFileReadable(const std::string &filePath)
{
    std::ifstream inStream(filePath);
    return inStream.good();
}

std::string readFile(const std::string &filePath)
{
    using namespace std;

    ifstream inStream(filePath);

    if (!inStream)
    {
        throw runtime_error("failed to read file (insufficient permisions?): " + filePath);
    }

    return static_cast<stringstream const&>(stringstream() << inStream.rdbuf()).str();
}

void dumpToFile(const std::string &text, const std::string &filePath, bool newline)
{
    std::ofstream outStream(filePath, std::ios_base::app);

    if (!outStream)
    {
        throw std::runtime_error("failed to write file (insufficient permisions?): " + filePath);
    }

    outStream << text;

    if (newline)
    {
        outStream << std::endl;
    }
}

bool removeFile(const std::string &filePath)
{
    return remove(filePath.c_str()) == 0;
}

int randIntRangeExcluded(int start, int end, int excluded)
{
    using namespace std;

    if (start > end or (start == end and start == excluded))
    {
        throw invalid_argument("range is empty");
    }

    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist(start, end);

    while (true)
    {
        int res = dist(mt);
        if (res != excluded)
        {
            return res;
        }
    }
}


namespace
{

template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
inline std::string toString(T val)
{
    return std::to_string(val);
}
inline std::string toString(const std::string &str)
{
    return str;
}

} // namespace (anonymous)

template<typename T>
std::string join(const std::vector<T> &vec, const std::string &delim)
{
    if (vec.empty())
    {
        return "";
    }

    std::string res = "";

    for (size_t i = 0; i < vec.size() - 1; ++i)
    {
        res += toString(vec[i]) + delim;
    }

    return res + toString(vec.back());
}

void removeEmptyStrings(std::vector<std::string> &vec)
{
    std::vector<std::string>::iterator it = vec.begin();

    for ( ; it != vec.end(); )
    {
        if (it->empty())
        {
            it = vec.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

std::string genRandomString(int size, const std::string &alphabet)
{   
    using namespace std;

    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist(0, alphabet.size() - 1);

    string res = "";

    for (int i = 0; i < size; ++i)
    {
        res += alphabet[dist(mt)];
    }

    return res;
}

std::string genRandomStringAlphNum(int size)
{
    using namespace std;

    /** Lookup table for alphanumeric characters. */
    static constexpr const char *alnumLUT = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    /** Size of alnumLUT exluding the terminating '\0'. */
    static constexpr int alnumLUTSize = 62;

    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist(0, alnumLUTSize - 1);

    string res = "";

    for (int i = 0; i < size; ++i)
    {
        res += alnumLUT[dist(mt)];
    }

    return res;
}

} // namespace sopang::helpers

#endif // HELPERS_HPP
