#ifndef DATAEXTRACTOR_H
#define DATAEXTRACTOR_H

#include <map>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <streambuf>

namespace fs = std::experimental::filesystem;

namespace {
    constexpr int       AFFIX_SIZE = 3;
    constexpr uintmax_t MAX_FILE_SIZE = 104857600;  // in bytes; 100 MB
    constexpr unsigned  BLOCK_SIZE = sizeof(char) * 5242880;
    constexpr unsigned  BUFFER_SIZE = 100;
    constexpr int       NUM_THREADS = 4;
}
// Class used to extract positions, prefixes and suffixes for all occurrences  of a 
// search string from file/files located at specified location; 
// If the location represent a directory, all files located inside it (including subdirectories)
// will be taken into account
class DataExtractor
{
public:
    enum AffixType
    {
        NO_AFFIX_TYPE,
        PREFIX,
        SUFFIX
    };  // Used by GetAvailableAffixChars()

    // Holds the prefix and the suffix for a position
    struct AffixData
    {
        void Clear();
        
        std::string prefix;
        std::string suffix;
    };

    // Holds affixes data for all positions of a search string  found inside a file
    typedef std::map< size_t, AffixData > StringData;

    // Holds the path of a file and the search string data found inside it
    struct FileData
    {
        FileData();

        FileData(fs::path Path, StringData Data);
        
        fs::path   path;
        StringData stringData;
    };
    
    static DataExtractor& instance(std::string SearchString, std::string Location);

    DataExtractor operator=(DataExtractor& d) = delete;

    DataExtractor(const DataExtractor& d) = delete;

    ~DataExtractor();

    // Populates the vector containing all files data
    void ExtractData();

    // Iterates through the  vector containing all files data and displays on the standard output,
    // for each file, the positions where the search string was found and the prefix and suffix 
    // associated with each position
    void DisplayData();

private:
    DataExtractor(std::string SearchString, std::string Location);

    // Finds search string positions inside a single file and their associated affixes
    void ExtractFileData(const fs::path& File, std::shared_ptr<FileData>& FileData);

    // Finds search string positions inside a single file and their associated affixes for files over 100 MB
    // Decreases memory usage but increases processing time
    void ExtractBigFileData(const fs::path& FileName, std::shared_ptr<FileData>& Data);

    // Obtains a list of files located at the specified location (recursively iterates through directories)
    std::vector<fs::path> GetFileList(const fs::path& Path);

    // Reads contents of a file in memory
    std::string ReadFile(fs::path) const;

    // Depending on type, prefix of suffix, computes the available length to be extracted
    size_t GetAvailableAffixChars(const AffixType Type, const size_t Pos, 
                                                         const size_t ContentsSize);

    // Extracts the prefix and the suffix associated to a specified position
    AffixData GetAffixData(const std::string& Contents, const size_t Pos);

    // Verifies if the search string was found in a file
    bool IsEmpty(const std::shared_ptr<FileData>& Data);

    // Properly displays a string containing special characters on standard output; 
    // (e.g. tabs will be displayed as '\t', newlines as '\n' etc.)
    void DisplayString(std::ostream& OutStream, const std::string& CppString);

    std::string           _searchString;
    std::string           _location;
    size_t                _searchStringSize;
    std::vector< std::shared_ptr<FileData> > _extractedData;
};

#endif // DATAEXTRACTOR_H
