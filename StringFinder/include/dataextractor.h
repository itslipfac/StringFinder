#ifndef DATAEXTRACTOR_H
#define DATAEXTRACTOR_H

#include <map>
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::experimental::filesystem;

class DataExtractor
{
public:
    //              < location,            <pref       , suf   >    > >
    typedef std::map< std::string, std::map<std::string, std::string> > StringData;

    static DataExtractor& instance(std::string SearchString, std::string Location);

    struct FileData
    {
        std::string path;
        StringData data;
    };

    DataExtractor operator=(DataExtractor& d) = delete;

    DataExtractor(const DataExtractor& d) = delete;

    ~DataExtractor();

    // extract final data
    std::vector<DataExtractor::FileData> ExtractData();

private:
    DataExtractor(std::string SearchString, std::string Location);

    // extract data from file
    DataExtractor::FileData ExtractFileData(fs::path File);

    // obtain files from directory
    std::vector<fs::path> GetFileList(fs::path Path);

    std::string     m_searchString;
    std::string     m_location;

};

#endif // DATAEXTRACTOR_H
