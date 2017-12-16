#include "commandparser.h"

#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::experimental::filesystem;

using namespace std;

CommandParser::CommandParser()
{

}

bool CommandParser::AreArgumentsValid(const int Argc, const char * const Argv[]) const
{
    bool areValid = false;

    if (Argc != ARGUMENTS_NUMBER)
    {
        cout << "Invalid number of arguments: " << Argc << endl;

        PrintHelp();
    }
    else if (IsPathValid(Argv[1]) && IsSearchStringValid(Argv[2]))
    {
        areValid = true;
    }
    else
    {
        cout << "Invalid arguments" << endl;
        PrintHelp();
    }

    return areValid;
}

void CommandParser::PrintArguments(const int Argc, const char * const Argv[]) const
{
    cout << "Number of arguments: " << Argc << endl;

    for (int i = 0; i < Argc; ++i)
    {
        cout << "argv["  << i << "] = " << Argv[i] << endl;
    }
}

bool CommandParser::HasValidLength(const char * const String, const int MinLength, const int MaxLength) const
{
    const int stringLength = strlen(String);
    bool      hasValidLength = ( (MinLength < stringLength) && (stringLength < MaxLength) );

    return hasValidLength;
}

bool CommandParser::IsPathValid(const char * const Path) const
{
    bool      isValid = false;
    const int pathLength = strlen(Path);
    fs::path  path(Path);

    if ( !HasValidLength(Path, PATH_MIN_LENGTH, PATH_MAX_LENGTH) )
    {
        cout << "Invalid path: " << path << ". Length: " << pathLength << " is invalid." << endl;
    }
    else if ( !fs::exists(path) )
    {
        cout << "Invalid path: " << Path << ". Location doesn't exit." << endl;
    }
    else if ( !fs::is_regular_file(path) && !fs::is_directory(path) )
    {
        cout << "Invalid path: " << path << ". Location exists, but is neither a regular file nor a directory" << endl;
    }
    else
    {
        cout << "Path valid." << endl;

        isValid = true;
    }

    return isValid;
}

bool CommandParser::IsSearchStringValid(const char * const SearchString) const
{
    bool      isValid = false;
    const int stringLength = strlen(SearchString);

    if ( !HasValidLength(SearchString, STRING_MIN_LENGTH, STRING_MAX_LENGTH) )
    {
        cout << "Invalid search string: " << SearchString << ". Length: " << stringLength << " is invalid." << endl;
    }
    else
    {
        cout << "Search string valid." << endl;

        isValid = true;
    }

    return isValid;
}

void CommandParser::PrintHelp() const
{
    cout << "Usage: ./StringFinder <path> <search_string>" << endl;
}



