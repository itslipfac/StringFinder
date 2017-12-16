#ifndef DATAEXTRACTOR_H
#define DATAEXTRACTOR_H

#include <map>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <streambuf>

namespace fs = std::experimental::filesystem;

class DataExtractor
{
public:
    enum AffixType
    {
        NO_AFFIX_TYPE,
        PREFIX,
        SUFFIX
    };

    struct AffixData
    {
        void Clear();
        
        std::string prefix;
        std::string suffix;
    };

    typedef std::map< size_t, AffixData > StringData;

    struct FileData
    {
        fs::path   path;
        StringData stringData;
    };
    
    static DataExtractor& instance(std::string SearchString, std::string Location);

    DataExtractor operator=(DataExtractor& d) = delete;

    DataExtractor(const DataExtractor& d) = delete;

    ~DataExtractor();

    // extract final data
    std::vector<DataExtractor::FileData> ExtractData();

private:
    DataExtractor(std::string SearchString, std::string Location);

    // extract data from file
    DataExtractor::FileData ExtractFileData(const fs::path& File);

    // obtain files from directory
    std::vector<fs::path> GetFileList(const fs::path& Path);

    // reads file contents
    std::string ReadFile(fs::path) const;

    // computes available prefix chars
    size_t GetAvailableAffixChars(const AffixType Type, const size_t Pos, 
                                  const size_t ContentsSize);

    // extracts the prefix and the suffix
    AffixData GetAffixData(const std::string& Contents, const size_t Pos);

    std::ostream& WriteString(std::ostream& OutStream, const std::string& CppString);

    std::string _searchString;
    std::string _location;
    size_t      _searchStringSize;
};

#endif // DATAEXTRACTOR_H
