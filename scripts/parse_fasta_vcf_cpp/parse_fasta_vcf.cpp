#include <cassert>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <Variant.h> // vcflib
#include <zstd.h>


using namespace std;

using SourcesMap = unordered_map<int, unordered_map<string, set<int>>>;

SourcesMap parseVcfFile(string filePath, string &chrID, int &sourceCount);
bool shouldProcessVariant(const vcflib::Variant &variant);

void parseFastaFile(const string &filePath, const SourcesMap &sourcesMap, const string &chrID);

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

    parseFastaFile(args.at(2), sourcesMap, chrID);

    cout << "All finished" << endl;
    return 0;
}

SourcesMap parseVcfFile(string filePath, string &chrID, int &sourceCount)
{
    cout << "Counting lines..." << endl;

    ifstream inStream(filePath);
    const int lineCount = count(istreambuf_iterator<char>(inStream), istreambuf_iterator<char>(), '\n');

    cout << "Initializing the parser..." << endl;

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
        const double processedPercentage = (100.0 * parsedCount++) / lineCount;
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
                assert(val.size() == 3);
                const std::vector<int> indexes({stoi(string(1, val[0])), stoi(string(1, val[2]))});

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
            if (alphabet.find(c) == string::npos)
                return false;
        }
    }

    return true;
}

void parseFastaFile(const string &filePath, const SourcesMap &sourcesMap, const string &chrID)
{
    std::ifstream inStream(filePath);
    string line;

    bool inGenome = false;

    while (getline(inStream, line))
    {
        if (line[0] == '>')
        {
            if (inGenome) // Encountered the next genome -> finish processing.
                break;

            const string curChrID = line.substr(1);

            if (curChrID == chrID)
            {
                inGenome = true;
                cout << "Entered genome = " << chrID;

                continue;
            }
        }
    }
}
