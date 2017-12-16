#ifndef COMMANDVALIDATOR_H
#define COMMANDVALIDATOR_H

namespace {
constexpr int ARGUMENTS_NUMBER = 3;
constexpr int PATH_MIN_LENGTH = 0;
constexpr int PATH_MAX_LENGTH = 128;
constexpr int STRING_MIN_LENGTH = 0;
constexpr int STRING_MAX_LENGTH = 128;
}

class CommandParser
{
public:
    CommandParser();

    bool AreArgumentsValid(const int Argc, const char * const Argv[]) const;

    void PrintArguments(const int Argc, const char * const Argv[]) const;

private:
    bool HasValidLength(const char * const String, const int MinLength, const int MaxLength) const;

    bool IsPathValid(const char * const Path) const;

    bool IsSearchStringValid(const char * const SearchString) const;

    void PrintHelp() const;
};

#endif // COMMANDVALIDATOR_H
