//	trax track library
//	AD 2026 
//
//  "the resolution of all the fruitless searches"
//
//								Peter Gabriel
//
// Copyright (c) 2026 Trend Redaktions- und Verlagsgesellschaft mbH
// Copyright (c) 2019 Marc-Michael Horstmann
//
// Permission is hereby granted to any person obtaining a copy of this software 
// and associated source code (the "Software"), to use, view, and study the 
// Software for personal or internal business purposes, subject to the following 
// conditions:
//
// 1. Redistribution, modification, sublicensing, or commercial use of the 
// Software is NOT permitted without prior written consent from the copyright 
// holder.
//
// 2. The Software is provided "AS IS", without warranty of any kind, express 
// or implied.
//
// 3. All copies of the Software must retain this license notice.
//
// For further information, please contact: horstmann@traxlibrary.dev

#pragma once

#include <cstddef>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace appframe
{

// A small, header-only command-line parser.
//
// The character type is a template parameter so the same code can parse
// both narrow (char) and wide (wchar_t) argument vectors. Two convenience
// aliases are provided below:
//
//   appframe::CommandLine   -> narrow  (int    main(int, char*[]))
//   appframe::WCommandLine  -> wide    (int   wmain(int, wchar_t*[]))
//
template <typename CharT>
class BasicCommandLine
{
public:
    using String     = std::basic_string<CharT>;
    using StringView = std::basic_string_view<CharT>;

    BasicCommandLine( int argc, CharT* argv[] )
    {
        Parse(argc, argv);
    }

    // ------------------------------------------------------------
    // Options
    // ------------------------------------------------------------

    // True if the option was specified.
    //
    // Examples:
    //   --verbose
    //   -v
    //
    bool Has( StringView option ) const
    {
        return m_Options.find(String(option)) != m_Options.end();
    }

    // Return the value of an option.
    //
    // Throws if the option does not exist or has no value.
    //
    // Examples:
    //   --input foo.eep
    //   --input=foo.eep
    //
    String Get( StringView option ) const
    {
        const auto it = m_Options.find(String(option));

        if (it == m_Options.end())
            throw std::runtime_error(
                "Missing command-line option: " + ToNarrow(option));

        if (it->second.empty())
            throw std::runtime_error(
                "Command-line option requires a value: " + ToNarrow(option));

        return it->second;
    }

    // Return the value if present, otherwise return defaultValue.
    String Get(
        StringView option,
        StringView defaultValue ) const
    {
        const auto it = m_Options.find(String(option));

        if (it == m_Options.end())
            return String(defaultValue);

        return it->second;
    }

    // True if the option exists and has a non-empty value.
    bool HasValue( StringView option ) const
    {
        const auto it = m_Options.find(String(option));

        return it != m_Options.end() && !it->second.empty();
    }

    // ------------------------------------------------------------
    // Typed values
    // ------------------------------------------------------------

    int GetInt(
        StringView option,
        int defaultValue = 0 ) const
    {
        if (!Has(option))
            return defaultValue;

        // std::stoi is overloaded for both std::string and std::wstring.
        return std::stoi(Get(option));
    }

    double GetDouble(
        StringView option,
        double defaultValue = 0.0 ) const
    {
        if (!Has(option))
            return defaultValue;

        // std::stod is overloaded for both std::string and std::wstring.
        return std::stod(Get(option));
    }

    bool GetBool(
        StringView option,
        bool defaultValue = false ) const
    {
        if (!Has(option))
            return defaultValue;

        const String value = Get(option);

        if (value.empty())
            return true;

        if (EqualsAscii(value, "true") || EqualsAscii(value, "TRUE") ||
            EqualsAscii(value, "yes")  || EqualsAscii(value, "YES")  ||
            EqualsAscii(value, "1"))
            return true;

        if (EqualsAscii(value, "false") || EqualsAscii(value, "FALSE") ||
            EqualsAscii(value, "no")    || EqualsAscii(value, "NO")    ||
            EqualsAscii(value, "0"))
            return false;

        throw std::runtime_error(
            "Invalid boolean value for " + ToNarrow(option) +
            ": " + ToNarrow(value));
    }

    // ------------------------------------------------------------
    // Positional arguments
    // ------------------------------------------------------------

    const std::vector<String>& Positional() const
    {
        return m_Positional;
    }

    std::size_t PositionalCount() const
    {
        return m_Positional.size();
    }

    const String& Positional( std::size_t index ) const
    {
        if (index >= m_Positional.size())
            throw std::runtime_error(
                "Missing positional command-line argument");

        return m_Positional[index];
    }

    // ------------------------------------------------------------
    // Program name
    // ------------------------------------------------------------

    const String& Program() const
    {
        return m_Program;
    }

private:

    // Character constants for the active character type.
    static constexpr CharT Dash  = static_cast<CharT>('-');
    static constexpr CharT Equal = static_cast<CharT>('=');

    void Parse(int argc, CharT* argv[])
    {
        if (argc > 0 && argv[0])
            m_Program = argv[0];

        for (int i = 1; i < argc; ++i)
        {
            String argument = argv[i];

            // ----------------------------------------------------
            // End of options:
            //
            // Everything following "--" is positional.
            // ----------------------------------------------------

            if (argument.size() == 2 &&
                argument[0] == Dash &&
                argument[1] == Dash)
            {
                for (++i; i < argc; ++i)
                    m_Positional.emplace_back(argv[i]);

                break;
            }

            // ----------------------------------------------------
            // Long option
            //
            // --verbose
            // --input foo.eep
            // --input=foo.eep
            // ----------------------------------------------------

            if (argument.size() >= 2 &&
                argument[0] == Dash &&
                argument[1] == Dash)
            {
                String option = argument.substr(2);

                if (option.empty())
                    throw std::runtime_error(
                        "Invalid command-line option: --");

                const std::size_t equal = option.find(Equal);

                if (equal != String::npos)
                {
                    const String name  = option.substr(0, equal);
                    const String value = option.substr(equal + 1);

                    AddOption(name, value);
                }
                else
                {
                    // If the next argument doesn't start with '-',
                    // treat it as this option's value.
                    if (i + 1 < argc &&
                        argv[i + 1] &&
                        argv[i + 1][0] != Dash)
                    {
                        AddOption(option, argv[++i]);
                    }
                    else
                    {
                        AddOption(option, String{});
                    }
                }

                continue;
            }

            // ----------------------------------------------------
            // Short option
            //
            // -v
            // -i foo.eep
            // -i=foo.eep
            // ----------------------------------------------------

            if (argument.size() >= 2 &&
                argument[0] == Dash)
            {
                String option = argument.substr(1);

                const std::size_t equal = option.find(Equal);

                if (equal != String::npos)
                {
                    const String name  = option.substr(0, equal);
                    const String value = option.substr(equal + 1);

                    AddOption(name, value);
                }
                else if (i + 1 < argc &&
                         argv[i + 1] &&
                         argv[i + 1][0] != Dash)
                {
                    AddOption(option, argv[++i]);
                }
                else
                {
                    AddOption(option, String{});
                }

                continue;
            }

            // ----------------------------------------------------
            // Positional argument
            // ----------------------------------------------------

            m_Positional.push_back(argument);
        }
    }

    void AddOption(
        const String& option,
        const String& value)
    {
        if (m_Options.find(option) != m_Options.end())
        {
            throw std::runtime_error(
                "Duplicate command-line option: --" + ToNarrow(option));
        }

        m_Options.emplace(option, value);
    }

    // Case-sensitive comparison of a value against an ASCII literal.
    // Works regardless of the underlying character type.
    static bool EqualsAscii(const String& value, const char* ascii)
    {
        std::size_t i = 0;

        for (; ascii[i] != '\0'; ++i)
        {
            if (i >= value.size() ||
                value[i] != static_cast<CharT>(ascii[i]))
                return false;
        }

        return i == value.size();
    }

    // Convert a string to a narrow std::string for diagnostic messages.
    // Assumes ASCII content, which is sufficient for option names/values
    // in error text.
    static std::string ToNarrow(StringView text)
    {
        std::string result;
        result.reserve(text.size());

        for (CharT c : text)
            result.push_back(static_cast<char>(c));

        return result;
    }

private:

    String                              m_Program;
    std::unordered_map<String, String>  m_Options;
    std::vector<String>                 m_Positional;
};

// Convenience aliases.
using CommandLine  = BasicCommandLine<char>;
using WCommandLine = BasicCommandLine<wchar_t>;

} // namespace appframe