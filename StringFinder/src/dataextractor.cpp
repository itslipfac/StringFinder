#include "dataextractor.h"

#include <iostream>
#include <termcolor\termcolor.hpp>

using namespace std;
using namespace termcolor;

namespace {
    constexpr int AFFIX_SIZE = 3;
}

void DataExtractor::AffixData::Clear()
{
    prefix.clear();
    suffix.clear();
}

DataExtractor &DataExtractor::instance(string SearchString, string Location)
{
    static DataExtractor dataExtractor{SearchString, Location};

    return dataExtractor;
}

DataExtractor::~DataExtractor()
{
    // clean data
}

DataExtractor::DataExtractor(string SearchString, string Location) : _searchString{ SearchString },
    _location{ Location }, _searchStringSize{ SearchString.size() }
{

}

// extract final data
vector<DataExtractor::FileData> DataExtractor::ExtractData()
{
    vector<FileData> finalData;

    fs::path path(_location);

    if ( !fs::exists(path) )
    {
        cout << red << "Location doesn't exit." << reset << endl;
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
DataExtractor::FileData DataExtractor::ExtractFileData(const fs::path& FileName)
{
    StringData   stringData{};
    const string contents = ReadFile(FileName);
    size_t       position = contents.find(_searchString);

    while (position != string::npos)
    {
        stringData[position] = GetAffixData(contents, position);

        position = contents.find(_searchString, position + _searchStringSize);
    }

    FileData fileData{ FileName, stringData };

    return fileData;
}

vector<fs::path> DataExtractor::GetFileList(const fs::path& Path)
{
    vector<fs::path> fileList;

    if ( !fs::exists(Path) )
    {
        cout << red << "Location doesn't exit." << reset << endl;
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

string DataExtractor::ReadFile(fs::path FileName) const
{
    ifstream contentStream(FileName);
    string contentString;

    contentStream.seekg(0, ios::end);
    contentString.reserve(contentStream.tellg());
    contentStream.seekg(0, ios::beg);

    contentString.assign(istreambuf_iterator<char>(contentStream),
                         istreambuf_iterator<char>());

    return contentString;
}

size_t DataExtractor::GetAvailableAffixChars(const AffixType Type, const size_t Pos,
                                             const size_t ContentsSize)
{
    size_t availableChars{ 0 };
    
    switch (Type)
    {
    case PREFIX:
    {
        availableChars = (Pos > 2) ? AFFIX_SIZE : Pos;
        break;
    }
    case SUFFIX:
    {
        size_t difference = ContentsSize - (Pos + _searchStringSize);
        
        availableChars = (difference > 2) ? AFFIX_SIZE : difference;
        break;
    }
    default:
    {
        cout << red << "Unsupported affix type." << reset << endl;
        break;
    }
    }

    return availableChars;
}

DataExtractor::AffixData DataExtractor::GetAffixData(const string& Contents, const size_t Pos)
{
    const size_t contentsSize = Contents.size();
    const size_t availablePrefixChars = GetAvailableAffixChars(PREFIX, Pos, contentsSize);
    const size_t availableSuffixChars = GetAvailableAffixChars(SUFFIX, Pos, contentsSize);

    AffixData affixData{ Contents.substr(Pos - availablePrefixChars, availablePrefixChars),
                         Contents.substr(Pos + _searchStringSize, availableSuffixChars) };

    return affixData;
}

ostream& DataExtractor::WriteString(ostream& OutStream, const string& CppString)
{
    for (auto ch : CppString)
    {
        switch (ch)
        {
        case '\'':
            OutStream << "\\'";
            break;

        case '\"':
            OutStream << "\\\"";
            break;

        case '\?':
            OutStream << "\\?";
            break;

        case '\\':
            OutStream << "\\\\";
            break;

        case '\a':
            OutStream << "\\a";
            break;

        case '\b':
            OutStream << "\\b";
            break;

        case '\f':
            OutStream << "\\f";
            break;

        case '\n':
            OutStream << "\\n";
            break;

        case '\r':
            OutStream << "\\r";
            break;

        case '\t':
            OutStream << "\\t";
            break;

        case '\v':
            OutStream << "\\v";
            break;

        default:
            OutStream << ch;
        }
    }

    return OutStream;
}


