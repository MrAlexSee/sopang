#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <Variant.h>

using namespace std;

using SourcesMap = unordered_map<int, unordered_map<string, vector<int>>>;
SourcesMap parseVcfFile(vcflib::VariantCallFile &inStream);

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        cerr << "Usage: " << argv[0] << " <input-chr.fa> <input-variants.vcf> <output-chr.edz> <output-sources.edsz>" << endl;
        return 1;
    }

    vcflib::VariantCallFile vcfParser;
    string inVcfFilePath(argv[1]);

    vcfParser.open(inVcfFilePath);

    SourcesMap sourcesMap = parseVcfFile(vcfParser);

    cout << "All finished" << endl;
    return 0;
}

SourcesMap parseVcfFile(vcflib::VariantCallFile &vcfParser)
{
    SourcesMap ret;

    cout << vcfParser.header << endl;

    return ret;
}
