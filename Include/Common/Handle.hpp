/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Handle

    For use with HandleMap container. Separate header for breaking dependencies.
*/

namespace Common
{
    template<typename StorageType>
    class HandleMap;

    template<typename StorageType>
    class Handle
    {
    public:
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
        friend HandleMap<StorageType>;

        explicit Handle(const ValueType identifier) :
            m_identifier(identifier)
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
        std::size_t operator()(const Common::Handle<Type>& handle) const noexcept
        {
            return handle.GetIdentifier();
        }
    };

    template<typename Type>
    struct hash<std::pair<Common::Handle<Type>, Common::Handle<Type>>>
    {
        std::size_t operator()(
            const std::pair<Common::Handle<Type>, Common::Handle<Type>>& pair) const noexcept
        {
            /*
                Use combined identifiers as hash for pair of handles.
                On x64, this turns two 32bit hashes into one that is 64bit.
                On x86, this tries to combine two 32bit hashes into one 32bit.
            */

            using ValueType = typename Common::Handle<Type>::ValueType;
            static_assert(sizeof(ValueType) == 4);

            if constexpr(sizeof(std::size_t) == 8)
            {
                return static_cast<std::size_t>(pair.first.GetIdentifier())
                    * std::numeric_limits<ValueType>::max()
                    + pair.second.GetIdentifier();
            }
            else
            {
                return Common::CombineHash(
                    pair.first.GetIdentifier(),
                    pair.second.GetIdentifier());
            }
        }
    };
}
