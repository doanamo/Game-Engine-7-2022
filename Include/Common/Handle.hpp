/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Handle

    For use with handle map. Separate header for breaking dependencies.
*/

namespace Common
{
    template<typename StorageType>
    class HandleMap;

    template<typename StorageType>
    class Handle
    {
    public:
        friend HandleMap<StorageType>;

        using ValueType = uint32_t;

        static constexpr ValueType MaximumIdentifier = std::numeric_limits<ValueType>::max();
        static constexpr ValueType MaximumVersion = std::numeric_limits<ValueType>::max();
        static constexpr ValueType InvalidIdentifier = 0;
        static constexpr ValueType StartingVersion = 0;

        Handle() = default;

        Handle(const Handle& other)
        {
            *this = other;
        }

        Handle& operator=(const Handle& other)
        {
            ASSERT(&other != this);
            m_identifier = other.m_identifier;
            m_version = other.m_version;
            return *this;
        }

        ValueType GetIdentifier() const
        {
            return m_identifier;
        }

        ValueType GetVersion() const
        {
            return m_version;
        }

        bool IsValid() const
        {
            return m_identifier != InvalidIdentifier;
        }

        bool operator==(const Handle& other) const
        {
            return m_identifier == other.m_identifier && m_version == other.m_version;
        }

        bool operator!=(const Handle& other) const
        {
            return m_identifier != other.m_identifier || m_version != other.m_version;
        }

        bool operator<(const Handle& other) const
        {
            return m_identifier < other.m_identifier;
        }

    private:
        Handle(ValueType identifier) :
            m_identifier(identifier),
            m_version(StartingVersion)
        {
        }

        void Invalidate()
        {
            ASSERT(IsValid(), "Invalidating invalid handle!");
            ++m_version;
        }

        ValueType m_identifier = InvalidIdentifier;
        ValueType m_version = StartingVersion;
    };
}

namespace std
{
    template<typename Type>
    struct hash<Common::Handle<Type>>
    {
        std::size_t operator()(const Common::Handle<Type>& handle) const
        {
            /*
                Simply use unique identifier as hash value.
            */

            return handle.GetIdentifier();
        }
    };

    template<typename Type>
    struct hash<std::pair<Common::Handle<Type>, Common::Handle<Type>>>
    {
        std::size_t operator()(
            const std::pair<Common::Handle<Type>, Common::Handle<Type>>& pair) const
        {
            /*
                Use combined identifiers as hash for pair of handles.
                This turns two 32bit integers into one that is 64bit.
                We assume std::size_t is 64bit, but it should be fine if it is not.
            */

            using ValueType = typename Common::Handle<Type>::ValueType;
            return (std::size_t)pair.first.GetIdentifier()
                * std::numeric_limits<ValueType>::max()
                + pair.second.GetIdentifier();
        }
    };
}
