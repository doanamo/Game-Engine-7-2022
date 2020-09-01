/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <deque>
#include <optional>
#include "Common/Utility.hpp"
#include "Common/Result.hpp"

/*
    Handle Map

    Generational list of handles, identified by unique integer
    and version. Handle identifier can be reused or invalidated
    by incrementing its version integer.

    Implements caching mechanism that ensures that there is always
    a rotating pool of free handles to avoid situations where single
    handle is reused repeatedly leading to too fast exhaustion of
    its available version values.

    See unit tests for example usage.
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

        bool operator==(const Handle<StorageType>& other) const
        {
            return m_identifier == other.m_identifier && m_version == other.m_version;
        }

        bool operator!=(const Handle<StorageType>& other) const
        {
            return m_identifier != other.m_identifier || m_version != other.m_version;
        }

        bool operator<(const Handle<StorageType>& other) const
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

    template<typename StorageType>
    class HandleMap
    {
    public:
        using HandleType = Handle<StorageType>;
        using HandleValueType = typename HandleType::ValueType;

        struct HandleEntry
        {
            HandleEntry(const HandleType& handle) :
                handle(handle)
            {
            }

            void Invalidate()
            {
                (&storage)->~StorageType();
                new (&storage) StorageType();
                handle.Invalidate();
                valid = false;
            }

            StorageType storage = {};
            HandleType handle = {};
            bool valid = false;
        };

        struct HandleEntryRef
        {
            HandleEntryRef() = default;
            HandleEntryRef(HandleEntry& reference) :
                handle(reference.handle),
                storage(&reference.storage)
            {
            }

            StorageType* storage = nullptr;
            const HandleType handle = {};
        };

        struct ConstHandleEntryRef
        {
            ConstHandleEntryRef() = default;
            ConstHandleEntryRef(const HandleEntry& reference) :
                handle(reference.handle),
                storage(&reference.storage)
            {
            }

            const StorageType* storage = nullptr;
            const HandleType handle = {};
        };

        using HandleList = std::vector<HandleEntry>;
        using FreeList = std::deque<HandleValueType>;
        using FreeListIterator = typename FreeList::iterator;

        enum class CreateHandleErrors
        {
            RequestedHandleAlreadyCreated,
        };

        using CreateHandleResult = Common::Result<HandleEntryRef, CreateHandleErrors>;

        enum class LookupHandleErrors
        {
            NotFound,
        };

        using LookupHandleResult = Common::Result<HandleEntryRef, LookupHandleErrors>;
        using LookupHandleConstResult = Common::Result<ConstHandleEntryRef, LookupHandleErrors>;

        enum class FindRequestedHandleErrors
        {
            NotFound,
            InvalidRequest,
            AlreadyCreated,
        };

        using FindRequestedHandleResult = Common::Result<FreeListIterator, FindRequestedHandleErrors>;

        template<bool ConstReference>
        class HandleIterator
        {
        public:
            using IteratorType = typename std::conditional_t<ConstReference, typename HandleList::const_iterator, typename HandleList::iterator>;
            using DereferenceReturnType = typename std::conditional_t<ConstReference, ConstHandleEntryRef, HandleEntryRef>;

        public:
            HandleIterator(IteratorType it, IteratorType end) :
                m_it(it), m_end(end)
            {
                if(!IsValid())
                {
                    AdvanceUntilValid();
                }
            }

            HandleIterator(const HandleIterator& iterator) :
                m_it(iterator.m_it),
                m_end(iterator.m_end)
            {
            }

            HandleIterator& operator++()
            {
                ASSERT(m_it != m_end, "Out of bounds handle iteration!");
                AdvanceUntilValid();
                return *this;
            }

            HandleIterator operator++(int)
            {
                ASSERT(m_it != m_end, "Out of bounds handle iteration!");
                HandleIterator<ConstReference> iterator(this);
                AdvanceUntilValid();
                return iterator;
            }

            DereferenceReturnType operator*() const
            {
                ASSERT(m_it->valid, "Dereferencing invalid handle entry!");
                return DereferenceReturnType(*m_it);
            }

            DereferenceReturnType operator->() const
            {
                ASSERT(m_it->valid, "Dereferencing invalid handle entry!");
                return DereferenceReturnType(*m_it);
            }

            bool operator==(const HandleIterator& other) const
            {
                return m_it == other.m_it;
            }

            bool operator!=(const HandleIterator& other) const
            {
                return m_it != other.m_it;
            }

        private:
            void AdvanceUntilValid()
            {
                while(m_it != m_end)
                {
                    ++m_it;

                    if(IsValid())
                        break;
                }
            }

            bool IsValid()
            {
                return m_it != m_end && m_it->valid;
            }

            IteratorType m_it;
            IteratorType m_end;
        };

        HandleMap(std::size_t cacheSize = 32) :
            m_cacheSize(cacheSize)
        {
        }

        CreateHandleResult CreateHandle(const HandleType handleRequest = HandleType())
        {
            // Finds or creates unique handle that is suitable for use.
            // Handle with specific identifier and version can be requested, but
            // only in cases when such handle is deserialized. It is not intended
            // to have arbitrary handle requests with any identifier and version.

            std::optional<FreeListIterator> freeHandleIterator;
            if(auto result = FindRequestedHandle(handleRequest))
            {
                freeHandleIterator = result.UnwrapSuccess();
            }
            else
            {
                switch(result.UnwrapFailure())
                {
                case FindRequestedHandleErrors::NotFound:
                case FindRequestedHandleErrors::InvalidRequest:
                    freeHandleIterator = AllocateFreeHandle(handleRequest);
                    break;

                case FindRequestedHandleErrors::AlreadyCreated:
                    return Common::Failure(CreateHandleErrors::RequestedHandleAlreadyCreated);
                }
            }

            std::size_t handleEntryIndex = *freeHandleIterator.value();
            m_freeList.erase(freeHandleIterator.value());

            HandleEntry& handleEntry = m_handles[handleEntryIndex];
            handleEntry.valid = true;

            if(handleRequest.IsValid())
            {
                ASSERT(handleEntry.handle.m_identifier == handleRequest.m_identifier);
                handleEntry.handle.m_version = handleRequest.m_version;
            }

            return Common::Success(HandleEntryRef(handleEntry));
        }

        LookupHandleResult LookupHandle(const HandleType handle)
        {
            if(HandleEntry* handleEntry = FetchHandleEntry(handle))
            {
                return Common::Success(HandleEntryRef(*handleEntry));
            }

            return Common::Failure(LookupHandleErrors::NotFound);
        }

        LookupHandleConstResult LookupHandle(const HandleType handle) const
        {
            if(const HandleEntry* handleEntry = FetchHandleEntry(handle))
            {
                return Common::Success(ConstHandleEntryRef(*handleEntry));
            }
            
            return Common::Failure(LookupHandleErrors::NotFound);
        }

        bool DestroyHandle(const HandleType handle)
        {
            // Invalidate handle entry by incrementing its version integer
            // and reconstruct associated object storage. Check if destroyed
            // handle has exhausted its version pool and retire it if needed by
            // not adding it to free list queue.

            if(HandleEntry* handleEntry = FetchHandleEntry(handle))
            {
                handleEntry->Invalidate();

                if(handleEntry->handle.GetVersion() != HandleType::MaximumVersion)
                {
                    m_freeList.push_back(handleEntry->handle.GetIdentifier() - 1);
                }
                else
                {
                    m_retiredHandles++;
                }

                return true;
            }

            return false;
        }

        HandleValueType GetValidHandleCount() const
        {
            return Common::NumericalCast<HandleValueType>(m_handles.size() - m_freeList.size() - m_retiredHandles);
        }

        HandleValueType GetUnusedHandleCount() const
        {
            return Common::NumericalCast<HandleValueType>(m_freeList.size());
        }

        HandleValueType GetRetiredHandleCount() const
        {
            return Common::NumericalCast<HandleValueType>(m_retiredHandles);
        }

        HandleIterator<false> begin()
        {
            return HandleIterator<false>(m_handles.begin(), m_handles.end());
        }

        HandleIterator<false> end()
        {
            return HandleIterator<false>(m_handles.end(), m_handles.end());
        }

        HandleIterator<true> begin() const
        {
            return HandleIterator<true>(m_handles.begin(), m_handles.end());
        }

        HandleIterator<true> end() const
        {
            return HandleIterator<true>(m_handles.end(), m_handles.end());
        }

    private:
        FindRequestedHandleResult FindRequestedHandle(const HandleType handleRequest)
        {
            // Find handle identifier in free list queue that matches requested handle.
            // Knowing the requested identifier, we can also tell if handle is already
            // in use if it has not been found in free list queue.

            if(!handleRequest.IsValid())
            {
                return Common::Failure(FindRequestedHandleErrors::InvalidRequest);
            }

            auto foundFreeEntry = std::find_if(m_freeList.begin(), m_freeList.end(),
                [handleRequest](const HandleValueType& index) -> bool
                {
                    return (index + 1) == handleRequest.GetIdentifier();
                }
            );

            if(foundFreeEntry != m_freeList.end())
            {
                HandleEntry& handleEntry = m_handles[*foundFreeEntry];
                if(handleEntry.handle.m_version > handleRequest.m_version)
                {
                    return Common::Failure(FindRequestedHandleErrors::AlreadyCreated);
                }

                return Common::Success(foundFreeEntry);
            }
            else
            {
                HandleValueType currentMaxIdentifier = (HandleValueType)m_handles.size();
                if(handleRequest.GetIdentifier() <= currentMaxIdentifier)
                {
                    return Common::Failure(FindRequestedHandleErrors::AlreadyCreated);
                }

                return Common::Failure(FindRequestedHandleErrors::NotFound);
            }
        }

        FreeListIterator AllocateFreeHandle(const HandleType handleRequest)
        {
            // Allocate free handles until suitable handle is found.
            // Number of cached free entires is maintained to prevent too
            // quick exhaustions of handles, which will lead to them being
            // retired. This function will always return a valid iterator.

            bool requestedHandle = handleRequest.IsValid();

            while(m_freeList.size() <= m_cacheSize || requestedHandle)
            {
                VERIFY(m_handles.size() != HandleType::MaximumIdentifier, "Maximum handle identifier limit has been reached!");

                HandleValueType newHandleIdentifier = Common::NumericalCast<HandleValueType>(m_handles.size() + 1);
                HandleEntry& newHandleEntry = m_handles.emplace_back(HandleType(newHandleIdentifier));

                HandleValueType newHandleEntryIndex = Common::NumericalCast<HandleValueType>(m_handles.size() - 1);
                m_freeList.push_back(newHandleEntryIndex);

                if(requestedHandle)
                {
                    if(handleRequest.GetIdentifier() == newHandleIdentifier)
                    {
                        return m_freeList.end() - 1;
                    }
                }
            }

            return m_freeList.begin();
        }

        const HandleEntry* FetchHandleEntry(const HandleType handle) const
        {
            // Retrieve handle entry using handle identifier that maps
            // to storage index. Ensure that handle versions match.

            if(handle.GetIdentifier() <= 0 || handle.GetIdentifier() > (HandleValueType)m_handles.size())
                return nullptr;

            const HandleEntry& handleEntry = m_handles[handle.GetIdentifier() - 1];
            if(handleEntry.handle.GetVersion() == handle.GetVersion())
            {
                return &handleEntry;
            }

            return nullptr;
        }

        HandleEntry* FetchHandleEntry(const HandleType handle)
        {
            return const_cast<HandleEntry*>(static_cast<const HandleMap<StorageType>&>(*this).FetchHandleEntry(handle));
        }

        HandleList m_handles;
        FreeList m_freeList;

        const std::size_t m_cacheSize = 0;
        std::size_t m_retiredHandles = 0;
    };
}

namespace std
{
    template<typename Type>
    struct hash<Common::Handle<Type>>
    {
        std::size_t operator()(const Common::Handle<Type>& handle) const
        {
            return handle.GetIdentifier();
        }
    };

    template<typename Type>
    struct hash<std::pair<Common::Handle<Type>, Common::Handle<Type>>>
    {
        std::size_t operator()(const std::pair<Common::Handle<Type>, Common::Handle<Type>>& pair) const
        {
            // Use combined identifiers as a hash.
            // This turns two 32bit integers into one that is 64bit.
            // We assume std::size_t is 64bit, but it should be fine if it is not.
            return (std::size_t)pair.first.GetIdentifier() * std::numeric_limits<typename Common::Handle<Type>::ValueType>::max() + pair.second.GetIdentifier();
        }
    };
}
