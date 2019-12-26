#include <algorithm>
#include <cassert>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
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

SourcesMap processVcfFile(string filePath, string &chrID, int &sourceCount);
bool shouldProcessVariant(const vcflib::Variant &variant);

/** Returns a pair of (text chars, source chars). **/
pair<string, string> processFastaFile(const string &filePath, const SourcesMap &sourcesMap, const string &chrID);
/** Returns a pair of (text chars, source chars) obtained from the current line. **/
pair<string, string> processLine(int &vcfPositionCount, int charIdx, const string &line, const SourcesMap &sourcesMap);
string packNumber(const int n);

void dumpFiles(const string &textChars, const string &sourceChars, const string &outTextFilePath, const string &outSourcesFilePath);
string zstdCompress(const string &data, int compressionLevel);

int main(int argc, char **argv)
{
    const vector<string> args(argv + 1, argv + argc);

    if (args.size() != 4)
    {
        cerr << "Usage: " << argv[0] << " <input-chr.fa> <input-variants.vcf> <output-chr.edz> <output-sources.edsz>" << endl;
        return 1;
    }

    cout.precision(2);

    string chrID;
    int sourceCount;

    cout << "Building the sources map (stage 1/2)..." << endl;
    SourcesMap sourcesMap = processVcfFile(args.at(1), chrID, sourceCount);

    if (sourcesMap.empty())
        return 1;

    cout << endl << "Processing the fasta file (stage 2/2)..." << endl;
    auto [textChars, sourceChars] = processFastaFile(args.at(0), sourcesMap, chrID);

    sourceChars = to_string(sourceCount) + "\n" + sourceChars;
    
    dumpFiles(textChars, sourceChars, args.at(2), args.at(3));

    cout << "All finished" << endl;
    return 0;
}

SourcesMap processVcfFile(string filePath, string &chrID, int &sourceCount)
{
    cout << "Counting lines..." << endl;

    ifstream inStream(filePath);
    const int lineCount = count(istreambuf_iterator<char>(inStream), istreambuf_iterator<char>(), '\n');

    cout << endl << "Processing the VCF file, #lines = " << lineCount << endl;

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

    int parsedCount = count(vcfParser.header.begin(), vcfParser.header.end(), '\n') + 1;
    int processedCount = 0, ignoredCount = 0;

    chrID = "";

    vcflib::Variant variant(vcfParser);

    while (vcfParser.getNextVariant(variant))
    {
        const double processedPercentage = (100.0 * ++parsedCount) / lineCount;
        cout << "\rVCF progress (stage 1/2): " << fixed << processedPercentage << "%" << flush;

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
                const vector<int> indexes({stoi(parts[0]), stoi(parts[1])});

                for (int diploidIndex = 0; diploidIndex < static_cast<int>(indexes.size()); ++diploidIndex)
                {
                    const int altIndex = indexes[diploidIndex];

                    if (altIndex == 0) // Reference sequence.
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

    cout << endl << "Sources map: variant #positions = " << ret.size() << " #sources = " << sourceCount << endl;
    cout << "Processed VCF #records = " << processedCount << ", #ignored = " << ignoredCount << endl;

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

pair<string, string> processFastaFile(const string &filePath, const SourcesMap &sourcesMap, const string &chrID)
{
    ifstream inStream(filePath);

    if (not inStream.good())
    {
        cerr << "Failed to open fasta file: " << filePath << endl;
        return {};
    }

    string line;
    bool inGenome = false;

    int vcfPositionCount = 0, genomeLineCount = 0;
    int charIdx = 0;

    string textChars = "", sourceChars = "";

    const int expectedSize = 1024 * 1024 * 1024;

    textChars.reserve(expectedSize);
    sourceChars.reserve(expectedSize);

    while (getline(inStream, line))
    {
        if (line[0] == '>')
        {
            if (inGenome) // Encountered the next genome -> finish processing.
            {
                cout << "Exited genome = " << chrID << endl;
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

        const auto &[curTextChars, curSourceChars] = processLine(vcfPositionCount, charIdx, line, sourcesMap);

        textChars += move(curTextChars);
        sourceChars += move(curSourceChars);

        charIdx += line.size();
        genomeLineCount += 1;
    }

    cout << "Finished parsing the fasta file, ED text size = " << textChars.size() << endl;
    cout << "Processed VCF #positions = " << vcfPositionCount << ", genome #lines = " << genomeLineCount << endl;

    return {move(textChars), move(sourceChars)};
}

pair<string, string> processLine(int &vcfPositionCount, int charIdx, const string &line, const SourcesMap &sourcesMap)
{
    string textChars = "", sourceChars = "";

    textChars.reserve(line.size());
    sourceChars.reserve(line.size());

    const char sourceSegmentStartMark = 127;

    for (const char curChar : line)
    {
        if (sourcesMap.count(charIdx) == 0)
        {
            textChars += string(1, toupper(curChar));
            charIdx += 1;

            continue;
        }

        textChars += "{";
        sourceChars += sourceSegmentStartMark;

        assert(not sourcesMap.at(charIdx).empty());

        for (const auto &[altSequence, sampleIndexes] : sourcesMap.at(charIdx))
        {
            {
            string altSequenceUpper(altSequence.size(), ' ');
            std::transform(altSequence.begin(), altSequence.end(), altSequenceUpper.begin(),
                [](char c) -> char { return toupper(c); });

            textChars += altSequenceUpper + ",";
            }

            {
            vector<int> sampleIndexesSorted(sampleIndexes.begin(), sampleIndexes.end());
            sort(sampleIndexesSorted.begin(), sampleIndexesSorted.end());

            assert(sampleIndexesSorted.size() >= 1);

            sourceChars += packNumber(sampleIndexesSorted.size());
            sourceChars += packNumber(sampleIndexesSorted[0]);

            for (int i = 1; i < static_cast<int>(sampleIndexesSorted.size()); ++i)
            {
                const int diff = sampleIndexesSorted[i] - sampleIndexesSorted[i - 1];
                assert(diff > 0);

                sourceChars += packNumber(diff);
            }
            }
        }

        textChars += string(1, toupper(curChar));
        textChars += "}";

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
    const int zstdCompressionLevel = 22;
    cout << endl << "Using zstd compression level = " << zstdCompressionLevel << endl;

    {
    cout << "Original text size = " << textChars.size() << endl;
        
    ofstream outTextStream(outTextFilePath);
    outTextStream << zstdCompress(textChars, zstdCompressionLevel);
    }

    {
    cout << "Original sources size = " << sourceChars.size() << endl;

    ofstream outSourcesStream(outSourcesFilePath);
    outSourcesStream << zstdCompress(sourceChars, zstdCompressionLevel);
    }

    cout << "Dumped to files: " << outTextFilePath << ' ' << outSourcesFilePath << endl;
}

string zstdCompress(const string &data, int compressionLevel)
{
    cout << "Compressing..." << endl;
    const double bufferSizeFactor = 1.5;

    const size_t bufferSize = bufferSizeFactor * ZSTD_compressBound(data.size());
    char *dstBuffer = new char[bufferSize];

    const size_t compressedSize = ZSTD_compress(dstBuffer, bufferSize, data.c_str(), data.size(), compressionLevel);
    assert(compressedSize <= bufferSize);

    if (ZSTD_isError(compressedSize))
    {
        cerr << "Zstd compression failed" << endl;
        return "";
    }

    const string ret(dstBuffer, dstBuffer + compressedSize);

    delete[] dstBuffer;
    return move(ret);
}
