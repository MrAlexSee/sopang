#include <cassert>
#include <cctype>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <Variant.h> // vcflib
#include <zstd.h>


using namespace std;

using SourcesMap = unordered_map<int, unordered_map<string, set<int>>>;

SourcesMap parseVcfFile(string filePath, string &chrID, int &sourceCount);
bool shouldProcessVariant(const vcflib::Variant &variant);

pair<string, string> parseFastaFile(const string &filePath, const SourcesMap &sourcesMap, const string &chrID);
pair<string, string> processLine(int &charIdx, int &vcfPositionCount, const string &line, const SourcesMap &sourcesMap);
string packNumber(const int n);

void dumpFiles(const string &textChars, const string &sourceChars, const string &outTextFilePath, const string &outSourcesFilePath);

int main(int argc, char **argv)
{
    const vector<string> args(argv + 1, argv + argc);

    if (args.size() != 4)
    {
        cerr << "Usage: " << argv[0] << " <input-chr.fa> <input-variants.vcf> <output-chr.edz> <output-sources.edsz>" << endl;
        return 1;
    }

    string chrID;
    int sourceCount;

    cout << "Building sources map..." << endl;
    SourcesMap sourcesMap = parseVcfFile(args.at(1), chrID, sourceCount);

    if (sourcesMap.empty())
        return 1;

    cout << "Parsing the fasta file..." << endl;

    string textChars, sourceChars;
    tie(textChars, sourceChars) = parseFastaFile(args.at(0), sourcesMap, chrID);

    sourceChars = to_string(sourceCount) + "\n" + sourceChars;
    
    dumpFiles(textChars, sourceChars, args.at(2), args.at(3));

    cout << "All finished" << endl;
    return 0;
}

SourcesMap parseVcfFile(string filePath, string &chrID, int &sourceCount)
{
    cout << "Counting lines..." << endl;

    ifstream inStream(filePath);
    const int lineCount = count(istreambuf_iterator<char>(inStream), istreambuf_iterator<char>(), '\n');

    cout << "Parsing the VCF file..." << endl;

    vcflib::VariantCallFile vcfParser;
    vcfParser.open(filePath);

    if (not vcfParser.is_open())
    {
        cerr << "Failed to open VCF file: " << filePath << endl;
        return {};
    }

    SourcesMap ret;

    map<string, int> sampleNameToIndex;
    int nextSampleIndex = 0;

    vcflib::Variant variant(vcfParser);

    int parsedCount = 0, processedCount = 0, ignoredCount = 0;
    chrID = "";

    cout.precision(2);

    while (vcfParser.getNextVariant(variant))
    {
        const double processedPercentage = (100.0 * ++parsedCount) / lineCount;
        cout << "\rRough progress: " << fixed << processedPercentage << "%" << flush;

        parsedCount += 1;

        if (shouldProcessVariant(variant) == false)
        {
            ignoredCount += 1;
            continue;
        }

        if (chrID.empty())
        {
            chrID = variant.sequenceName;
        }
        else if (chrID != variant.sequenceName)
        {
            cerr << "Mismatching chromosome IDs" << endl;
            return {};
        }

        unordered_map<string, set<int>> curSources;

        for (const auto &[sampleName, sampleValues] : variant.samples)
        {
            for (const string &val : sampleValues.at("GT"))
            {
                vector<string> parts;
                boost::algorithm::split(parts, val, boost::is_any_of("|"));

                assert(parts.size() == 2);
                const std::vector<int> indexes({stoi(parts[0]), stoi(parts[1])});

                for (int diploidIndex = 0; diploidIndex < static_cast<int>(indexes.size()); ++diploidIndex)
                {
                    const int altIndex = indexes[diploidIndex];

                    if (altIndex == 0)
                        continue;

                    assert(altIndex >= 1 and altIndex <= static_cast<int>(variant.alt.size()));
                    const string &altSequence = variant.alt[altIndex - 1];

                    const string fullSampleName = sampleName + "_" + to_string(diploidIndex);

                    if (sampleNameToIndex.count(fullSampleName) == 0)
                    {
                        sampleNameToIndex[fullSampleName] = nextSampleIndex++;
                    }

                    curSources[altSequence].insert(sampleNameToIndex[fullSampleName]);
                }
            }
        }

        if (not curSources.empty())
        {
            if (ret.count(variant.position) == 0)
            {
                ret[variant.position] = curSources;
            }
            else
            {
                for (const auto &[altSequence, sampleIndexes] : curSources)
                {
                    if (ret[variant.position].count(altSequence) == 0)
                    {
                        ret[variant.position][altSequence] = sampleIndexes;
                    }
                    else
                    {
                        ret[variant.position][altSequence].insert(sampleIndexes.begin(), sampleIndexes.end());
                    }
                }
            }
        }

        processedCount += 1;
    }

    sourceCount = nextSampleIndex;

    cout << endl << "Processed VCF #records = " << processedCount << " #ignored = " << ignoredCount << endl;
    return ret;
}

bool shouldProcessVariant(const vcflib::Variant &variant)
{
    const string alphabet = "ACGTN";

    vector<string> sequences = variant.alt;
    sequences.push_back(variant.ref);

    for (const string &sequence : sequences)
    {
        for (const char c : sequence)
        {
            if (alphabet.find(toupper(c)) == string::npos)
                return false;
        }
    }

    return true;
}

pair<string, string> parseFastaFile(const string &filePath, const SourcesMap &sourcesMap, const string &chrID)
{
    std::ifstream inStream(filePath);

    if (not inStream.good())
    {
        cerr << "Failed to open fasta file: " << filePath << endl;
        return {};
    }

    string line;
    bool inGenome = false;

    int vcfPositionCount = 0, lineCount = 0;
    int charIdx = 0;

    string textChars = "", sourceChars = "";

    while (getline(inStream, line))
    {
        if (line[0] == '>')
        {
            if (inGenome) // Encountered the next genome -> finish processing.
            {
                cout << "Exited genome" << endl;
                break;
            }

            vector<string> parts;
            boost::algorithm::split(parts, line, boost::is_any_of(" "));

            string curID = parts[0].substr(1);
            boost::trim(curID);

            if (curID == chrID)
            {
                inGenome = true;
                cout << "Entered genome = " << chrID << endl;

                continue;
            }
        }

        if (not inGenome)
            continue;

        const auto &[curTextChars, curSourceChars] = processLine(charIdx, vcfPositionCount, line, sourcesMap);

        textChars += curTextChars;
        sourceChars += curSourceChars;

        charIdx += line.size();
        lineCount += 1;
    }

    cout << endl << "Finished parsing the fasta file, ED text size = " << textChars.size() << endl;
    cout << "Processed VCF #positions = " << vcfPositionCount << " lines = " << lineCount << endl;

    return {move(textChars), move(sourceChars)};
}

pair<string, string> processLine(int &charIdx, int &vcfPositionCount, const string &line, const SourcesMap &sourcesMap)
{
    string textChars = "", sourceChars = "";
    const char sourceSegmentStartMark = 127;

    for (const char curChar : line)
    {
        if (sourcesMap.count(charIdx) == 0)
        {
            textChars += toupper(curChar);
            charIdx += 1;

            continue;
        }

        textChars += "{";
        sourceChars += sourceSegmentStartMark;

        for (const auto &[altSequence, sampleIndexes] : sourcesMap.at(charIdx))
        {
            textChars += altSequence + ",";

            vector<int> sampleIndexesSorted(sampleIndexes.begin(), sampleIndexes.end());
            sort(sampleIndexesSorted.begin(), sampleIndexesSorted.end());

            sourceChars += packNumber(sampleIndexesSorted.size());
            sourceChars += packNumber(sampleIndexesSorted[0]);

            for (int i = 1; i < static_cast<int>(sampleIndexesSorted.size()); ++i)
            {
                const int diff = sampleIndexesSorted[i] - sampleIndexesSorted[i - 1];
                assert(diff > 0);

                sourceChars += packNumber(diff);
            }
        }

        textChars += toupper(curChar) + "}";
        charIdx += 1;

        vcfPositionCount += 1;
    }

    return {move(textChars), move(sourceChars)};
}

string packNumber(const int n)
{
    assert(n >= 0 and n < 16'384);

    if (n < 128)
    {
        return {static_cast<char>(128 + n)};
    }
    else
    {
        return {static_cast<char>(n / 128), static_cast<char>(128 + (n % 128))};
    }
}

void dumpFiles(const string &textChars, const string &sourceChars, const string &outTextFilePath, const string &outSourcesFilePath)
{
    cout << "Original text size = " << textChars.size() << endl;
    
    ofstream outTextStream(outTextFilePath);
    outTextStream << textChars;

    cout << "Original sources size = " << sourceChars.size() << endl;

    ofstream outSourcesStream(outSourcesFilePath);
    outSourcesStream << sourceChars;

    cout << "Dumped to files: " << outTextFilePath << ' ' << outSourcesFilePath << endl;
}
