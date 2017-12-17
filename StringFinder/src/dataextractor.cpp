#include "dataextractor.h"

#include <iostream>
#include <algorithm>
#include <omp.h>
#include <termcolor\termcolor.hpp>

using namespace std;
using namespace termcolor;

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
    _extractedData.clear();
}

DataExtractor::DataExtractor(string SearchString, string Location) : _searchString{ SearchString },
    _location{ Location }, _searchStringSize{ SearchString.size() }
{
}

void DataExtractor::ExtractData()
{
    fs::path path(_location);

    if ( !fs::exists(path) )
    {
        cout << red << "Location doesn't exit." << reset << endl;
    }
#pragma omp parallel num_threads(4)
    {
        // obtains files located at the specified path
        vector<fs::path> fileList = GetFileList(path);
#pragma omp for
        // extract data from each file
        for (int i = 0; i < fileList.size(); ++i)
        {
            shared_ptr<FileData> fileData{};
            uintmax_t            fileSize{ 0 };

            try
            {
                fileSize = fs::file_size(fileList[i]);
            }
            catch (fs::filesystem_error& e)
            {
                std::cout << red << e.what() << reset << endl;
                continue;
            }
            
            if (fileSize < MAX_FILE_SIZE)
            {
                ExtractFileData(fileList[i], fileData);
            }
            else
            {
                ExtractBigFileData(fileList[i], fileData);
            }

            if (!IsEmpty(fileData))
            {
                _extractedData.push_back(fileData);
            }
        }
    }
}

void DataExtractor::DisplayData()
{
    size_t numberOfFiles = _extractedData.size();

    if (0 == numberOfFiles)
    {
        cout << "No results to display for provided location: " << _location << endl;
    }
    else
    {
        cout << "Displaying data for search string: <" << green << _searchString << reset << "> found in: <" 
             << green << numberOfFiles << reset << ( (1 == numberOfFiles) ? "> file." : "> files." ) << endl;

        for (auto&& fileData : _extractedData)
        {
            cout << "Displaying data found inside <" << green << fileData->path << reset << ">:" << endl;

            for (auto&& value : fileData->stringData)
            {
                cout << "Position: " << green << value.first << reset;
                cout << "\t\tPrefix: ";
                DisplayString(cout, value.second.prefix);
                cout << "\tSuffix: ";
                DisplayString(cout, value.second.suffix);
                cout << endl;
            }

            cout << endl;
        }
    }
}

void DataExtractor::ExtractFileData(const fs::path& FileName, shared_ptr<FileData>& Data)
{
    StringData   stringData{};
    const string contents = ReadFile(FileName);
    size_t       position = contents.find(_searchString);

    while (position != string::npos)
    {
        stringData[position] = GetAffixData(contents, position);

        // search starting from next character
        position = contents.find(_searchString, ++position);
    }

    Data = make_shared<FileData>(FileName, stringData);
}

void DataExtractor::ExtractBigFileData(const fs::path& FileName, shared_ptr<FileData>& Data)
{
    StringData stringData{};
    char       block[BLOCK_SIZE] = { 0 };
    string     blockString{};
    char       buffer[BUFFER_SIZE] = { 0 };
    string     bufferString{};
    ifstream   contentStream(FileName);
    int        blocksCount = 0;

    // get content length
    contentStream.seekg (0, contentStream.end);
    streamoff length = contentStream.tellg();
    contentStream.seekg (0, contentStream.beg);
    
    while (contentStream.good())
    {
        // reset block contents
        fill(block, block + BLOCK_SIZE, 0);

        // read block by block
        if (BLOCK_SIZE <= length)
        {
            // also read a buffer from the previous block to make sure no results were omitted
            if (blocksCount > 0)
            {
                // ... start reading from the proper position 
                contentStream.seekg( blocksCount * (BLOCK_SIZE - BUFFER_SIZE) );
                contentStream.read(block, BLOCK_SIZE);
                contentStream.read(buffer, BUFFER_SIZE);
            }
            else
            {
                contentStream.read(block, BLOCK_SIZE);
            }

            blockString.assign(block, block + BLOCK_SIZE);
            bufferString.assign(buffer, buffer + BUFFER_SIZE);
        }
        else
        {
            contentStream.read(block, length);
            blockString.assign(block, length);
        }      

        size_t position = blockString.find(_searchString);

        // find occurrences of search string in current block
        while (position != string::npos)
        {
            size_t positionInFile = ( ( blocksCount * (BLOCK_SIZE - BUFFER_SIZE) ) + position );

            if (0 == blocksCount)
            {
                // this is the first block
                stringData[positionInFile] = GetAffixData(blockString, position);
            }
            else
            {
                stringData[positionInFile] = GetAffixData(bufferString + blockString, position + BUFFER_SIZE);
            }

            // search starting from next character
            position = blockString.find(_searchString, ++position);
        }

        ++blocksCount;
    }

    Data = make_shared<FileData>(FileName, stringData);
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
    string   contentString{};

    if (contentStream.good())
    {
        // get length of file
        contentStream.seekg(0, ios::end);
        contentString.reserve(contentStream.tellg());
        contentStream.seekg(0, ios::beg);

        // read data
        contentString.assign(istreambuf_iterator<char>(contentStream),
                             istreambuf_iterator<char>());
    }
    else
    {
        cout << red << "File: " << FileName << " cannot be open.";
    }

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

bool DataExtractor::IsEmpty(const shared_ptr<FileData>& Data)
{ 
    return (Data->stringData.size() == 0);
}

void DataExtractor::DisplayString(ostream& OutStream, const string& CppString)
{
    OutStream << green;
    
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

    OutStream << reset;
}

DataExtractor::FileData::FileData() : path{}, stringData{}
{
}

DataExtractor::FileData::FileData(fs::path Path, StringData Data) : path{ Path }, stringData{ Data }
{
}
