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

    static string readFile(const string &filePath)
    {
        ifstream inStream(filePath);

        if (!inStream)
            throw runtime_error("failed to read file: " + filePath);

        return static_cast<stringstream const&>(stringstream() << inStream.rdbuf()).str();
    }

    static void dumpToFile(const string &text, const string &filePath, bool newline = false)
    {
        ofstream outStream(filePath, ios_base::app);

        if (!outStream)
            throw runtime_error("failed to write file: " + filePath);

        outStream << text;

        if (newline)
            outStream << endl;
    }

    template<typename T>
    static void calcStatsMedian(const vector<T> &throughputVec, T *throughputMedian)
    {
        vector<T> tmp = throughputVec;

        sort(tmp.begin(), tmp.end());
        *throughputMedian = tmp[tmp.size() / 2];
    }
};

#endif // HELPERS_HPP
