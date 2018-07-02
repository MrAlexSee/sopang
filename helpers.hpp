#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <cmath>
#include <cstdio>
#include <fstream>
#include <random>
#include <stdexcept>
#include <string>
#include <sstream>
#include <type_traits>

using namespace std;

namespace sopang
{

struct Helpers
{
    Helpers() = delete;

    /*
     *** COLLECTIONS
     */

    /** Calculates a median from [vec] and stores it in [median]. */
    template<typename T>
    static void calcStatsMedian(const vector<T> &vec, T *median);
 
    /*
     *** FILES
     */

    inline static bool isFileReadable(const string &filePath);
    inline static string readFile(const string &filePath);

    /** Appends [text] to file with [filePath] followed by an optional newline if [newline] is true. */
    inline static void dumpToFile(const string &text, const string &filePath, bool newline = false);
    inline static bool removeFile(const string &filePath);

    /*
     *** RANDOM
     */

    /** Returns a random integer from range [start] to [end] (both inclusive) not equal to [excluded]. */
    inline static int randIntRangeExcluded(int start, int end, int excluded);

    /*
     *** STRINGS
     */

    template<typename T>
    static string join(const vector<T> &vec, const string &delim);
    inline static void removeEmptyStrings(vector<string> &vec);

    /** Returns a random string having [size] characters sampled uniformly from [alphabet]. */
    inline static string genRandomString(int size, string alphabet);
    /** Returns a random alphanumeric string having [size] characters. */
    inline static string genRandomStringAlphNum(int size);
   
private:
    template<typename T, typename = typename enable_if<is_arithmetic<T>::value, T>::type>
    inline static string toString(T val)
    {
        return to_string(val);
    }
    inline static string toString(const string &str)
    {
        return str;
    }

    /** Lookup table for alphanumeric characters. */
    static constexpr const char *alnumLUT = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    /** Size of alnumLUT exluding the terminating '\0'. */
    static constexpr int alnumLUTSize = 62;
};

template<typename T>
void Helpers::calcStatsMedian(const vector<T> &vec, T *median)
{
    if (vec.size() == 0)
    {
        return;
    }

    vector<T> tmp = vec;

    sort(tmp.begin(), tmp.end());
    *median = tmp[tmp.size() / 2];
}

bool Helpers::isFileReadable(const string &filePath)
{
    ifstream inStream(filePath);
    return inStream.good();
}

string Helpers::readFile(const string &filePath)
{
    ifstream inStream(filePath);

    if (!inStream)
    {
        throw runtime_error("failed to read file (insufficient permisions?): " + filePath);
    }

    return static_cast<stringstream const&>(stringstream() << inStream.rdbuf()).str();
}

void Helpers::dumpToFile(const string &text, const string &filePath, bool newline)
{
    ofstream outStream(filePath, ios_base::app);

    if (!outStream)
    {
        throw runtime_error("failed to write file (insufficient permisions?): " + filePath);
    }

    outStream << text;

    if (newline)
    {
        outStream << endl;
    }
}

bool Helpers::removeFile(const string &filePath)
{
    return remove(filePath.c_str()) == 0;
}

int Helpers::randIntRangeExcluded(int start, int end, int excluded)
{
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

template<typename T>
string Helpers::join(const vector<T> &vec, const string &delim)
{
    if (vec.size() == 0)
    {
        return "";
    }

    string res = "";

    for (size_t i = 0; i < vec.size() - 1; ++i)
    {
        res += toString(vec[i]) + delim;
    }

    return res + toString(vec.back());
}

void Helpers::removeEmptyStrings(vector<string> &vec)
{
    vector<string>::iterator it = vec.begin();

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

string Helpers::genRandomString(int size, string alphabet)
{   
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

string Helpers::genRandomStringAlphNum(int size)
{
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

} // namespace sopang

#endif // HELPERS_HPP
