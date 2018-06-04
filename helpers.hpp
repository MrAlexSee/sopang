#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <cmath>
#include <cstdio>
#include <fstream>
#include <random>
#include <string>
#include <sstream>
#include <type_traits>

using namespace std;

namespace inverted_basilisk
{

struct Helpers
{
    Helpers() = delete;

    /*
     *** FILES
     */

    static bool isFileReadable(const string &filePath)
    {
        ifstream inStream(filePath);
        return inStream.good();
    }

    static string readFile(const string &filePath)
    {
        ifstream inStream(filePath);

        if (!inStream)
        {
            throw runtime_error("failed to read file (insufficient permisions?): " + filePath);
        }

        return static_cast<stringstream const&>(stringstream() << inStream.rdbuf()).str();
    }

    static void dumpToFile(const string &text, const string &filePath, bool newline = false)
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

    inline static bool removeFile(const string &filePath)
    {
        return remove(filePath.c_str()) == 0;
    }

    /*
     *** COLLECTIONS
     */

    template<typename T>
    static void calcStatsMedian(const vector<T> &throughputVec, T *throughputMedian)
    {
        if (throughputVec.size() == 0)
        {
            return;
        }

        vector<T> tmp = throughputVec;

        sort(tmp.begin(), tmp.end());
        *throughputMedian = tmp[tmp.size() / 2];
    }

    /*
     *** STRINGS
     */

    template<typename T>
    static string join(const vector<T> &vec, const string &delim)
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

    static void removeEmptyStrings(vector<string> &vec)
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

    static string genRandomString(int size, string alphabet)
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

    static string genRandomStringAlphNum(int size)
    {
        random_device rd;
        mt19937 mt(rd());
        uniform_int_distribution<int> dist(0, chLUTSize - 1);

        string res = "";

        for (int i = 0; i < size; ++i)
        {
            res += chLUT[dist(mt)];
        }

        return res;
    }

private:
    template<typename T, typename = typename enable_if<is_arithmetic<T>::value, T>::type>
    static string toString(T val)
    {
        return to_string(val);
    }
    inline static string toString(const string &str)
    {
        return str;
    }

    static constexpr const char *chLUT = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static constexpr int chLUTSize = 62;
};

}

#endif // HELPERS_HPP
