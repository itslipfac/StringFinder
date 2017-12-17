#include "commandparser.h"

#include <iostream>
#include <filesystem>
#include <termcolor\termcolor.hpp>

namespace fs = std::experimental::filesystem;

using namespace std;
using namespace termcolor;

CommandParser::CommandParser() : _location{}, _searchString{}
{
}

bool CommandParser::ValidateArguments(const int Argc, const char * const Argv[])
{
    bool areValid = false;

    if (Argc != ARGUMENTS_NUMBER)
    {
        cout << red << "Invalid number of arguments: " << Argc << reset << endl;

        PrintHelp();
    }
    else if ( IsPathValid(Argv[1]) && IsSearchStringValid(Argv[2]) )
    {
        areValid = true;

        _location.assign(Argv[1]);
        _searchString.assign(Argv[2]);
    }
    else
    {
        cout << red << "Invalid arguments" << reset << endl;
        PrintHelp();
    }

    return areValid;
}

void CommandParser::PrintArguments(const int Argc, const char * const Argv[]) const
{
    cout << yellow << "Number of arguments: " << Argc << reset << endl;

    for (int i = 0; i < Argc; ++i)
    {
        cout << yellow << "argv["  << i << "] = " << Argv[i] << reset << endl;
    }
}

std::string CommandParser::location() const
{
    return _location;
}

std::string CommandParser::searchString() const
{
    return _searchString;
}

bool CommandParser::HasValidLength(const char * const String, const int MinLength, const int MaxLength) const
{
    const size_t stringLength = strlen(String);
    const bool   hasValidLength = ( (MinLength < stringLength) && (stringLength < MaxLength) );

    return hasValidLength;
}

bool CommandParser::IsPathValid(const char * const Path) const
{
    bool           isValid = false;
    const size_t   pathLength = strlen(Path);
    const fs::path path(Path);

    if ( !HasValidLength(Path, PATH_MIN_LENGTH, PATH_MAX_LENGTH) )
    {
        cout << red << "Invalid path: " << path << ". Length: " << pathLength << " is invalid." << reset << endl;
    }
    else if ( !fs::exists(path) )
    {
        cout << red << "Invalid path: " << Path << ". Location doesn't exit." << reset << endl;
    }
    else if ( !fs::is_regular_file(path) && !fs::is_directory(path) )
    {
        cout << red << "Invalid path: " << path << ". Location exists, but is neither a regular file nor a directory" << reset << endl;
    }
    else
    {
        cout << green << "Path valid." << reset << endl;

        isValid = true;
    }

    return isValid;
}

bool CommandParser::IsSearchStringValid(const char * const SearchString) const
{
    bool         isValid = false;
    const size_t stringLength = strlen(SearchString);

    if ( !HasValidLength(SearchString, STRING_MIN_LENGTH, STRING_MAX_LENGTH) )
    {
        cout << red << "Invalid search string: " << SearchString << ". Length: " << stringLength << " is invalid." << reset << endl;
    }
    else if ( !ContainsOnlyValidChars(SearchString) )
    {
        cout << red << "Invalid search string: " << SearchString << ". It contains invalid characters." << reset << endl;
    }
    else
    {
        cout << green << "Search string valid." << reset << endl;

        isValid = true;
    }

    return isValid;
}

bool CommandParser::ContainsOnlyValidChars(const char * const SearchString) const
{
    // TODO: add logic as per requirements

    return true;
}

void CommandParser::PrintHelp() const
{
    cout << yellow << "Usage: StringFinder.exe <path> <search_string>" << reset << endl;
}
