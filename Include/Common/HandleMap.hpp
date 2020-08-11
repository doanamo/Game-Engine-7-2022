/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <deque>

/*
    Handle Set

    Generational list of handles, identified by unique integer
    and version, held along with their values in a sparse array.
*/

namespace Common
{
    template<typename Type>
    class HandleMap;

    template<typename Type>
    class Handle
    {
    public:
        friend HandleMap<Type>;

        using ValueType = uint32_t;
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

        bool operator==(const Handle<Type>& other) const
        {
            return m_identifier == other.m_identifier && m_version == other.m_version;
        }

        bool operator!=(const Handle<Type>& other) const
        {
            return m_identifier != other.m_identifier || m_version != other.m_version;
        }

        bool operator<(const Handle<Type>& other) const
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

    template<typename Type>
    class HandleMap
    {
    public:
        using ObjectType = Type;
        using HandleType = Handle<Type>;
        using HandleValueType = typename HandleType::ValueType;

        struct HandleEntry
        {
            HandleEntry(const HandleType& handle) :
                handle(handle)
            {
            }

            HandleType handle;
            ObjectType object = {};
            bool valid = false;
        };

        template<bool ConstReference>
        struct HandleEntryRef
        {
            using HandleEntryType = typename std::conditional_t<ConstReference, const HandleEntry, HandleEntry>;
            using ObjectType = typename std::conditional_t<ConstReference, const Type, Type>;

            HandleEntryRef() = default;
            HandleEntryRef(HandleEntryType& reference) :
                handle(reference.handle),
                valid(reference.valid),
                object(&reference.object)
            {
            }

            const HandleType handle;
            ObjectType* object = nullptr;
            const bool valid = false;
        };

        using HandleList = std::vector<HandleEntry>;
        using FreeList = std::deque<HandleValueType>;

        template<bool ConstReference>
        class HandleIterator
        {
        public:
            using IteratorType = typename std::conditional_t<ConstReference, typename HandleList::const_iterator, typename HandleList::iterator>;
            using DereferenceReturnType = typename HandleEntryRef<ConstReference>;

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
        HandleMap() = default;

        HandleEntryRef<false> CreateHandle(const HandleType handleRequest = HandleType())
        {
            // Next free list entry index that we want to use.
            // Initially pointing at invalid end element and needs to be found or created.
            auto freeEntry = m_freeList.end();

            // Check if requested handle identifier is in free list queue.
            bool requestedHandle = handleRequest.IsValid();

            if(requestedHandle)
            {
                // Find handle index in free list queue that interests us.
                freeEntry = std::find_if(m_freeList.begin(), m_freeList.end(),
                    [handleRequest](const HandleValueType& index)
                    {
                        return (index + 1) == handleRequest.GetIdentifier();
                    }
                );

                // Could not find requested identifier in free list queue.
                // Check if handle with requested identifier is already in use.
                if(freeEntry == m_freeList.end())
                {
                    // We did not find free list entry with this identifier, so there is a chance it is already being used.
                    HandleValueType currentMaxIdentifier = (HandleValueType)m_handles.size();
                    if(handleRequest.GetIdentifier() <= currentMaxIdentifier)
                    {
                        // We are unable to create requested handle because it's already in use.
                        ASSERT(false, "Requested handle already in use!");
                        return HandleEntryRef<false>();
                    }
                }
            }

            // Create new handles if free list is empty or until we get requested handle.
            if(freeEntry == m_freeList.end())
            {
                while(m_freeList.empty() || requestedHandle)
                {
                    // Create handle entry with new index.
                    HandleValueType newHandleIdentifier = (HandleValueType)(m_handles.size() + 1);
                    m_handles.emplace_back(HandleType(newHandleIdentifier));

                    // Add new object entry to free list.
                    std::size_t currentObjectEntryCount = m_handles.size();
                    m_freeList.push_back((HandleValueType)(currentObjectEntryCount - 1));

                    // Reset invalidated iterator.
                    freeEntry = m_freeList.end();

                    // Check if this is our requested identifier.
                    if(requestedHandle)
                    {
                        if(handleRequest.GetIdentifier() == newHandleIdentifier)
                        {
                            // We got our requested handle, so pop it next.
                            // We can step back using end iterator as we know free list is not empty at this point.
                            freeEntry = m_freeList.end() - 1;
                            break;
                        }
                    }
                }

                // Check if we have our handle candidate.
                if(freeEntry == m_freeList.end())
                {
                    // We may still not have a valid free entry selected, so choose free entry in front.
                    freeEntry = m_freeList.begin();
                }
            }

            // Get free handle entry from free list queue.
            HandleEntry& handleEntry = m_handles[*freeEntry];

            // Set requested handle version.
            // There is no harm in bumping handle version by an arbitrary value (unless it overflows).
            if(requestedHandle)
            {
                if(handleEntry.handle.m_version > handleRequest.m_version)
                {
                    ASSERT(false, "Requesting handle that will result in handle reuse!");
                    return HandleEntryRef<false>();
                }

                handleEntry.handle.m_version = handleRequest.m_version;
            }

            // Mark handle entry as valid.
            handleEntry.valid = true;

            // Erase used index from free list queue.
            m_freeList.erase(freeEntry);

            // Return handle entry reference.
            return HandleEntryRef<false>(handleEntry);
        }

        HandleEntryRef<false> LookupHandle(HandleType handle)
        {
            HandleEntry* handleEntry = FetchHandleEntry(handle);

            if(handleEntry != nullptr)
            {
                return HandleEntryRef<false>(*handleEntry);

            }
            else
            {
                return HandleEntryRef<false>();
            }
        }

        bool DestroyHandle(HandleType handle)
        {
            // Fetch handle entry.
            HandleEntry* handleEntry = FetchHandleEntry(handle);
            if(handleEntry == nullptr)
                return false;

            // Invalidate object handle.
            handleEntry->handle.Invalidate();
            handleEntry->valid = false;

            // Reset handle object.
            (&handleEntry->object)->~Type();
            new (&handleEntry->object) Type();

            // Add entry index to free list.
            ASSERT(handleEntry->handle.GetIdentifier() != 0);
            m_freeList.push_back(handleEntry->handle.GetIdentifier() - 1);

            // Successfully removed handle.
            return true;
        }

        HandleValueType GetValidHandleCount() const
        {
            return (HandleValueType)(m_handles.size() - m_freeList.size());
        }

        HandleValueType GetUnusedHandleCount() const
        {
            return (HandleValueType)m_freeList.size();
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
        HandleEntry* FetchHandleEntry(HandleType handle)
        {
            // Make sure identifier is within handle array's range and return null otherwise.
            if(handle.GetIdentifier() <= 0 || handle.GetIdentifier() > (HandleValueType)m_handles.size())
                return nullptr;

            // Make sure handle versions are matching.
            HandleEntry& handleEntry = m_handles[handle.GetIdentifier() - 1];

            if(handle.GetVersion() == handleEntry.handle.GetVersion())
            {
                return &handleEntry;
            }
            else
            {
                return nullptr;
            }
        }

        HandleList m_handles; // List of handles and type storage.
        FreeList m_freeList;  // List of free handles that can be reused.
    };
}
