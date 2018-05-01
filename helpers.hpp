#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <cmath>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

struct Helpers
{
    Helpers() = delete;

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
};

#endif // HELPERS_HPP
