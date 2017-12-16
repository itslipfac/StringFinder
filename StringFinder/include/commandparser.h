#ifndef COMMANDVALIDATOR_H
#define COMMANDVALIDATOR_H

#include <string>

namespace {
constexpr int ARGUMENTS_NUMBER = 3;
constexpr int PATH_MIN_LENGTH = 0;
constexpr int PATH_MAX_LENGTH = 128;
constexpr int STRING_MIN_LENGTH = 0;
constexpr int STRING_MAX_LENGTH = 128;
}

// Class used to validate command line arguments and transform them in a
// suitable format for processing
// Provides getters for location and search string
class CommandParser
{
public:
    CommandParser();

    // Validate location and search string
    bool ValidateArguments(const int Argc, const char * const Argv[]);

    // Used for debugging
    void PrintArguments(const int Argc, const char * const Argv[]) const;

    std::string location() const;

    std::string searchString() const;

private:
    // Validates that a string has the length between min and max limits
    bool HasValidLength(const char * const String, const int MinLength, const int MaxLength) const;

    // Validates that the provided path is a file or a directory
    bool IsPathValid(const char * const Path) const;

    // Validates the string length and content
    bool IsSearchStringValid(const char * const SearchString) const;

    bool ContainsOnlyValidChars(const char * const SearchString) const;

    // Show application usage
    void PrintHelp() const;

    std::string _location;
    std::string _searchString;
};

#endif // COMMANDVALIDATOR_H
