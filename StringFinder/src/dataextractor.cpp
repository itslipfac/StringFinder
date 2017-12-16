#include "dataextractor.h"

#include <iostream>

using namespace std;

DataExtractor &DataExtractor::getInstance(string SearchString, string Location)
{
    static DataExtractor dataExtractor{SearchString, Location};

    return dataExtractor;
}

DataExtractor::~DataExtractor()
{
    // clean data
}

DataExtractor::DataExtractor(string SearchString, string Location) : m_searchString{SearchString},
    m_location{Location}
{

}

// extract final data
vector<DataExtractor::FileData> DataExtractor::ExtractData()
{
    vector<FileData> finalData;

    fs::path path(m_location);

    if ( !fs::exists(path) )
    {
        cout << "Location doesn't exit." << endl;
        return finalData;
    }

    // extract file list from the location provided; recursively iterate directories
    vector<fs::path> fileList = GetFileList(path);

    // extract data from each file
    for (auto&& fileName : fileList)
    {
        finalData.push_back( ExtractFileData(fileName) );
    }

    return finalData;
}

// extract data from file
DataExtractor::FileData DataExtractor::ExtractFileData(fs::path FileName)
{
    FileData fileData;

    // add logic
    if ( fs::is_regular_file(FileName) )
    {
        cout << "File: " << FileName.filename() << endl;
    }
    else if ( fs::is_directory(FileName) )
    {
       cout << "Dir: " << FileName.filename() << endl;
    }

    return fileData;
}

vector<fs::path> DataExtractor::GetFileList(fs::path Path)
{
    vector<fs::path> fileList;

    if ( !fs::exists(Path) )
    {
        cout << "Location doesn't exit." << endl;
        return fileList;
    }

    if ( fs::is_regular_file(Path) )
    {
        fileList.push_back(Path);
    }
    else if ( fs::is_directory(Path) )
    {
        fs::recursive_directory_iterator recursiveIter(Path);
        fs::recursive_directory_iterator endIter;

        for (; recursiveIter != endIter; ++recursiveIter)
        {
            if ( fs::is_regular_file(*recursiveIter) )
            {
                fileList.push_back(recursiveIter->path());
            }
        }
    }

    return fileList;
}
