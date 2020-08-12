/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <deque>
#include "Common/Utility.hpp"

/*
    Handle Set

    Generational list of handles, identified by unique integer
    and version, held along with their values in a sparse array.
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

    public:
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

        // Unique identifier that can be reused.
        ValueType m_identifier = InvalidIdentifier;

        // Unique version for each identifier
        // that gets incremented when invalidated.
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

            StorageType storage = {};
            HandleType handle;
            bool valid = false;
        };

        struct HandleEntryRef
        {
            HandleEntryRef() = default;
            HandleEntryRef(HandleEntry& reference) :
                handle(reference.handle),
                valid(reference.valid),
                storage(&reference.storage)
            {
            }

            StorageType* storage = nullptr;
            const HandleType handle;
            const bool valid = false;
        };

        struct ConstHandleEntryRef
        {
            ConstHandleEntryRef() = default;
            ConstHandleEntryRef(const HandleEntry& reference) :
                handle(reference.handle),
                valid(reference.valid),
                storage(&reference.storage)
            {
            }

            const StorageType* storage = nullptr;
            const HandleType handle;
            const bool valid = false;
        };

        using HandleList = std::vector<HandleEntry>;
        using FreeList = std::deque<HandleValueType>;

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
                ASSERT(m_it != m_end, "Out of bounds iteration!");
                AdvanceUntilValid();
                return *this;
            }

            HandleIterator operator++(int)
            {
                ASSERT(m_it != m_end, "Out of bounds iteration!");
                HandleIterator<ConstReference> iterator(this);
                AdvanceUntilValid();
                return iterator;
            }

            DereferenceReturnType operator*() const
            {
                return DereferenceReturnType(*m_it);
            }

            DereferenceReturnType operator->() const
            {
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
                // Moves iterator forward to next valid element.
                while(m_it != m_end)
                {
                    ++m_it;

                    if(IsValid())
                        break;
                }
            }

            bool IsValid()
            {
                // Check if current iterator position is valid.
                return m_it != m_end && m_it->valid;
            }

            IteratorType m_it;
            IteratorType m_end;
        };

    public:
        HandleMap(std::size_t cacheSize = 32) :
            m_cacheSize(cacheSize)
        {
        }

        HandleEntryRef CreateHandle(const HandleType handleRequest = HandleType())
        {
            // Next free list entry index that we want to use.
            // Initially pointing at invalid end element and needs to be found or created.
            auto freeEntryIterator = m_freeList.end();
            bool foundFreeEntry = false;

            // Check if requested handle identifier is in free list queue.
            bool requestedHandle = handleRequest.IsValid();

            if(requestedHandle)
            {
                // Find handle identifier in free list queue that interests us.
                freeEntryIterator = std::find_if(m_freeList.begin(), m_freeList.end(),
                    [handleRequest](const HandleValueType& index)
                    {
                        return (index + 1) == handleRequest.GetIdentifier();
                    }
                );

                // Could not find requested identifier in free list queue.
                // Check if handle with requested identifier is already in use.
                if(freeEntryIterator == m_freeList.end())
                {
                    // We did not find free list entry with this identifier, so there is a chance it is already being used.
                    HandleValueType currentMaxIdentifier = (HandleValueType)m_handles.size();
                    if(handleRequest.GetIdentifier() <= currentMaxIdentifier)
                    {
                        // We are unable to create requested handle because it's already in use.
                        ASSERT(false, "Requested handle already in use!");
                        return HandleEntryRef();
                    }
                }
                else
                {
                    foundFreeEntry = true;
                }
            }

            // Remove free handles that exhausted their version pool.
            while(!m_freeList.empty())
            {
                HandleEntry* handleEntry = &m_handles[*m_freeList.begin()];

                if(handleEntry->handle.GetVersion() == HandleType::MaximumVersion)
                {
                    // Do not use this handle anymore and attempt to find another candidate.
                    // Discarding handle will waste tiny amount of memory, but this will happen very rarely.
                    // Popping from front does not invalidate iterators for dequeue.
                    m_freeList.pop_front();
                }
                else
                {
                    // We only need to check the first free handle.
                    break;
                }
            }

            // Create new handles if free list is empty or until we get requested handle.
            if(!foundFreeEntry)
            {
                while(m_freeList.size() <= m_cacheSize)
                {
                    // Check if we have reached the numerical limits.
                    VERIFY(m_handles.size() != HandleType::MaximumIdentifier, "Maximum handle identifier limit has been reached!");

                    // Create handle entry with new index.
                    HandleValueType newHandleIdentifier = Common::NumericalCast<HandleValueType>(m_handles.size() + 1);
                    m_handles.emplace_back(HandleType(newHandleIdentifier));

                    // Add new object entry to free list.
                    HandleValueType newHandleEntryIndex = Common::NumericalCast<HandleValueType>(m_handles.size() - 1);
                    m_freeList.push_back(newHandleEntryIndex);

                    // Check if this is our requested identifier.
                    if(requestedHandle)
                    {
                        if(handleRequest.GetIdentifier() == newHandleIdentifier)
                        {
                            // We got our requested handle, so pop it next.
                            // We can step back using end iterator as we know free list is not empty at this point.
                            freeEntryIterator = m_freeList.end() - 1;
                            foundFreeEntry = true;

                            // We cannot continue creating handles for cache as allocations will invalidate our iterator.
                            break;
                        }
                    }
                }
            }

            // Choose first free handle if none was found yet.
            if(!foundFreeEntry)
            {
                freeEntryIterator = m_freeList.begin();
                foundFreeEntry = true;
            }

            // Get free handle entry from free list queue.
            HandleEntry& handleEntry = m_handles[*freeEntryIterator];

            // Set requested handle version.
            // There is no harm in bumping handle version by an arbitrary value (unless it overflows).
            if(requestedHandle)
            {
                if(handleEntry.handle.m_version > handleRequest.m_version)
                {
                    ASSERT(false, "Requesting handle that will result in handle reuse!");
                    return HandleEntryRef();
                }

                handleEntry.handle.m_version = handleRequest.m_version;
            }

            // Mark handle entry as valid.
            handleEntry.valid = true;

            // Erase used index from free list queue.
            m_freeList.erase(freeEntryIterator);

            // Return handle entry reference.
            return HandleEntryRef(handleEntry);
        }

        HandleEntryRef LookupHandle(const HandleType handle)
        {
            HandleEntry* handleEntry = FetchHandleEntry(handle);

            if(handleEntry != nullptr)
            {
                return HandleEntryRef(*handleEntry);

            }
            else
            {
                return HandleEntryRef();
            }
        }

        ConstHandleEntryRef LookupHandle(const HandleType handle) const
        {
            const HandleEntry* handleEntry = FetchHandleEntry(handle);

            if(handleEntry != nullptr)
            {
                return ConstHandleEntryRef(*handleEntry);
            }
            else
            {
                return ConstHandleEntryRef();
            }
        }

        bool DestroyHandle(const HandleType handle)
        {
            // Fetch handle entry.
            HandleEntry* handleEntry = FetchHandleEntry(handle);
            if(handleEntry == nullptr)
                return false;

            // Invalidate object handle.
            handleEntry->handle.Invalidate();
            handleEntry->valid = false;

            // Reset handle object.
            (&handleEntry->storage)->~StorageType();
            new (&handleEntry->storage) StorageType();

            // Add entry index to free list.
            ASSERT(handleEntry->handle.GetIdentifier() != 0);
            m_freeList.push_back(handleEntry->handle.GetIdentifier() - 1);

            // Successfully removed handle.
            return true;
        }

        HandleValueType GetValidHandleCount() const
        {
            return Common::NumericalCast<HandleValueType>(m_handles.size() - m_freeList.size());
        }

        HandleValueType GetUnusedHandleCount() const
        {
            return Common::NumericalCast<HandleValueType>(m_freeList.size());
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
        HandleEntry* FetchHandleEntry(const HandleType handle)
        {
            return const_cast<HandleEntry*>(static_cast<const HandleMap<StorageType>&>(*this).FetchHandleEntry(handle));
        }

        const HandleEntry* FetchHandleEntry(const HandleType handle) const
        {
            // Make sure identifier is within handle array's range and return null otherwise.
            if(handle.GetIdentifier() <= 0 || handle.GetIdentifier() > (HandleValueType)m_handles.size())
                return nullptr;

            // Make sure handle versions are matching.
            const HandleEntry& handleEntry = m_handles[handle.GetIdentifier() - 1];

            if(handle.GetVersion() == handleEntry.handle.GetVersion())
            {
                return &handleEntry;
            }
            else
            {
                return nullptr;
            }
        }

        // List of handles and type storage.
        HandleList m_handles;

        // List of free handles that can be reused.
        FreeList m_freeList;

        // Size of cached free list handles.
        // Safe guard for handle exhaustion.
        std::size_t m_cacheSize = 64;
    };
}

namespace std
{
    // Handle hash functor.
    template<typename StorageType>
    struct hash<Common::Handle<StorageType>>
    {
        std::size_t operator()(const Common::Handle<StorageType>& handle) const
        {
            // Use identifier as a hash.
            return handle.GetIdentifier();
        }
    };

    // Handle pair hash functor.
    template<typename StorageType>
    struct hash<std::pair<Common::Handle<StorageType>, Common::Handle<StorageType>>>
    {
        std::size_t operator()(const std::pair<Common::Handle<StorageType>, Common::Handle<StorageType>>& pair) const
        {
            // Use combined identifiers as a hash.
            // This turns two 32bit integers into one that's 64bit.
            // We assume std::size_t is 64bit, but it is fine if it is not.
            return (std::size_t)pair.first.GetIdentifier() * std::numeric_limits<Common::Handle::ValueType>::max() + pair.second.GetIdentifier();
        }
    };
}
