/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Config Types
*/

namespace Core
{
    /*
        Base
    */

    using ConfigVariablePair = std::pair<Common::Name, std::string>;
    using ConfigVariableArray = std::vector<ConfigVariablePair>;

    template<typename Type>
    class ConfigValueType
    {
    public:
        static constexpr bool IsSupported()
        {
            return false;
        }
    };

    /*
        Boolean
    */

    template<>
    class ConfigValueType<bool>
    {
    public:
        static std::string Format(const bool& value)
        {
            return value ? "true" : "false";
        }

        static Common::Result<bool, void> Parse(const std::string& value)
        {
            return Common::Success(value == "true");
        }

        static constexpr bool IsSupported()
        {
            return true;
        }
    };

    /*
        Integer
    */

    template<>
    class ConfigValueType<int>
    {
    public:
        static std::string Format(const int& value)
        {
            return std::to_string(value);
        }

        static Common::Result<int, void> Parse(const std::string& value)
        {
            try
            {
                return Common::Success(std::stoi(value));
            }
            catch(...)
            {
                return Common::Failure();
            }
        }

        static constexpr bool IsSupported()
        {
            return true;
        }
    };

    /*
        Single Floating Point Precision
    */

    template<>
    class ConfigValueType<float>
    {
    public:
        static std::string Format(const float& value)
        {
            return std::to_string(value);
        }

        static Common::Result<float, void> Parse(const std::string& value)
        {
            try
            {
                return Common::Success(std::stof(value));
            }
            catch(...)
            {
                return Common::Failure();
            }
        }

        static constexpr bool IsSupported()
        {
            return true;
        }
    };

    /*
        Double Floating Point Precision
    */

    template<>
    class ConfigValueType<double>
    {
    public:
        static std::string Format(const double& value)
        {
            return std::to_string(value);
        }

        static Common::Result<double, void> Parse(const std::string& value)
        {
            try
            {
                return Common::Success(std::stod(value));
            }
            catch(...)
            {
                return Common::Failure();
            }
        }

        static constexpr bool IsSupported()
        {
            return true;
        }
    };

    /*
        Character String
    */

    template<>
    class ConfigValueType<std::string>
    {
    public:
        static std::string Format(const std::string& value)
        {
            return value;
        }

        static Common::Result<std::string, void> Parse(const std::string& value)
        {
            return Common::Success(value);
        }

        static constexpr bool IsSupported()
        {
            return true;
        }
    };
}
