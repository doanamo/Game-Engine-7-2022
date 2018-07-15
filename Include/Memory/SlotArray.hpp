/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <deque>

/*
    Element Handle

    Used to reference elements in slot array container.
*/

namespace Common
{
    // Element handle structure.
    template<typename Type>
    struct ElementHandle
    {
        // Type declarations
        using ValueType = std::size_t;

        static const ValueType MaximumIdentifier = std::numeric_limits<ValueType>::max();
        static const ValueType MaximumVersion = std::numeric_limits<ValueType>::max();

        // Constructor.
        ElementHandle(ValueType identifier = 0, ValueType version = 0) :
            identifier(identifier),
            version(version)
        {
        }

        // Sorting operator.
        bool operator<(const ElementHandle& other) const
        {
            if(identifier < other.identifier)
                return true;

            if(identifier == other.identifier)
                return version < other.version;

            return false;
        }

        // Comparison operators.
        bool operator==(const ElementHandle& other) const
        {
            return identifier == other.identifier && version == other.version;
        }

        bool operator!=(const ElementHandle& other) const
        {
            return identifier != other.identifier || version != other.version;
        }

        // Handle data.
        ValueType identifier;
        ValueType version;
    };
}

namespace std
{
    // Entity handle hash functor.
    template<typename Type>
    struct hash<Common::ElementHandle<Type>>
    {
        std::size_t operator()(const Common::ElementHandle<Type>& handle) const
        {
            // Use the identifier as a hash.
            return handle.identifier;
        }
    };

    // Entity handle pair hash functor.
    template<typename Type>
    struct hash<std::pair<Common::ElementHandle<Type>, Common::ElementHandle<Type>>>
    {
        std::size_t operator()(const std::pair<Common::ElementHandle<Type>, Common::ElementHandle<Type>>& pair) const
        {
            // Use combined identifiers as a hash.
            // This turns two 32bit integers into one that is 64bit.
            // We assume std::size_t is 64bit, but it is fine if it's less.
            return pair.first.identifier * std::numeric_limits<int>::max() + pair.second.identifier;
        }
    };
}

/*
    Slot Array

    Efficient container of objects in allocated pools that does not invalidate references.
*/

namespace Common
{
    // Slot array class.
    template<typename ElementType>
    class SlotArray
    {
    public:
        // Type declarations and constant definitions.
        using HandleType = ElementHandle<ElementType>;
        using ValueType = typename HandleType::ValueType;

        static const ValueType InvalidIdentifier = 0;
        static const ValueType StartingIdentifier = 1;
        static const ValueType StartingVersion = 0;

        // Element entry structure.
        struct ElementEntry
        {
            // Constructor.
            ElementEntry() :
                exists(false),
                handleVersion(StartingVersion),
                nextFreeEntry(InvalidIdentifier)
            {
            }

            // Flag indicating wherever element exists.
            bool exists;

            // Pointer to object in the data table.
            ElementType element;

            // Version of the associated handle.
            ValueType handleVersion;

            // Intrusive free list pointer to the free next entry.
            ValueType nextFreeEntry;
        };

        using ElementEntryList = std::deque<ElementEntry>;

        // Iterator class.
        class Iterator
        {
        public:
            // Constructor.
            Iterator(SlotArray* container = nullptr, ValueType identifier = InvalidIdentifier) :
                m_container(container),
                m_identifier(identifier)
            {
            }

            // Returns the current handle.
            HandleType GetHandle()
            {
                if(m_container)
                {
                    ValueType index = m_identifier - m_container->m_startingIdentifier;

                    if(index < m_container->m_entries.size())
                    {
                        return HandleType(m_identifier, m_container->m_entries[index].handleVersion);
                    }
                }

                // Return an invalid handle.
                return HandleType();
            }

            // Iterator operators.
            ElementType& operator*()
            {
                ASSERT(m_container != nullptr, "Iterator has an invalid container!");
                ASSERT(m_identifier != InvalidIdentifier, "Iterator has an invalid identifier!");

                ElementEntry& elementEntry = m_container->m_entries[m_identifier - m_container->m_startingIdentifier];
                ASSERT(elementEntry.exists, "Iterator is pointing at non existing element!");

                return elementEntry.element;
            }

            ElementType* operator->()
            {
                ASSERT(m_container != nullptr, "Iterator has an invalid container!");
                ASSERT(m_identifier != InvalidIdentifier, "Iterator has an invalid identifier!");

                ElementEntry& elementEntry = m_container->m_entries[m_identifier - m_container->m_startingIdentifier];
                ASSERT(elementEntry.exists, "Iterator is pointing at non existing element!");

                return &elementEntry.element;
            }

            Iterator& operator++()
            {
                ASSERT(m_container != nullptr, "Iterator has an invalid container!");
                ASSERT(m_identifier != InvalidIdentifier, "Iterator has an invalid identifier!");

                // Find next existing element.
                for(std::size_t i = m_identifier + 1 - m_container->m_startingIdentifier; i < m_container->m_entries.size(); ++i)
                {
                    if(m_container->m_entries[i].exists)
                    {
                        m_identifier = i + m_container->m_startingIdentifier;
                        return *this;
                    }
                }

                // We did not find any valid elements and reached the end.
                this->Invalidate();

                return *this;
            }

            // Comparison operators.
            bool operator==(const Iterator& other) const
            {
                return m_container == other.m_container && m_identifier == other.m_identifier;
            }

            bool operator!=(const Iterator& other) const
            {
                return m_container != other.m_container || m_identifier != other.m_identifier;
            }

        private:
            // Invalidates the iterator instance.
            void Invalidate()
            {
                m_container = nullptr;
                m_identifier = InvalidIdentifier;
            }

        private:
            // Iterator bookmark.
            SlotArray* m_container;
            ValueType m_identifier;
        };

    public:
        // Constructor.
        SlotArray() :
            m_elementCount(0),
            m_startingIdentifier(StartingIdentifier),
            m_freeListDequeue(InvalidIdentifier),
            m_freeListEnqueue(InvalidIdentifier)
        {
        }

        // Creates a new element and returns its handle.
        template<typename... Arguments>
        HandleType Create(Arguments&&... arguments)
        {
            // Check if we have reached the numerical limits.
            VERIFY(m_entries.size() != HandleType::MaximumIdentifier, "Handle identifier limit has been reached!");

            // Check if the next entry on the free list is valid.
            while(m_freeListDequeue != InvalidIdentifier)
            {
                // Retrieve first free entry from the free list.
                int entryIndex = m_freeListDequeue - m_startingIdentifier;
                ElementEntry& elementEntry = m_entries[entryIndex];

                // Ensure that the entry does not exist.
                ASSERT(!elementEntry.exists, "Handle on the free list is already being used!");

                // Check if entry has exhausted its possible versions.
                if(elementEntry.handleVersion == HandleType::MaximumVersion)
                {
                    // Remove element entry from the free list.
                    if(m_freeListDequeue == m_freeListEnqueue)
                    {
                        // If there was only one entry in the queue,
                        // set the free list queue state to empty.
                        m_freeListDequeue = InvalidIdentifier;
                        m_freeListEnqueue = InvalidIdentifier;
                    }
                    else
                    {
                        // If there were more than a single element in the queue,
                        // set the beginning of the queue to the next free element.
                        m_freeListDequeue = elementEntry.nextFreeEntry;
                    }

                    // Clear next free identifier from the exhausted element entry.
                    elementEntry.nextFreeEntry = InvalidIdentifier;

                    // Attempt to find another candidate for a new entity handle.
                    continue;
                }

                // Found a good candidate for a new entity handle.
                break;
            }

            // Create a new entry if the free list queue is empty.
            if(m_freeListDequeue == InvalidIdentifier)
            {
                // Create a new element entry.
                m_entries.emplace_back();

                // Add new element entry to the free list queue.
                m_freeListDequeue = m_startingIdentifier + m_entries.size() - 1;
                m_freeListEnqueue = m_startingIdentifier + m_entries.size() - 1;
            }

            // Retrieve an unused entry from the free list.
            ValueType entryIndex = m_freeListDequeue - m_startingIdentifier;
            ElementEntry& elementEntry = m_entries[entryIndex];

            // Remove unused handle from the free list.
            if(m_freeListDequeue == m_freeListEnqueue)
            {
                // If there was only one element in the queue,
                // set the free list queue state to empty.
                m_freeListDequeue = InvalidIdentifier;
                m_freeListEnqueue = InvalidIdentifier;
            }
            else
            {
                ASSERT(elementEntry.nextFreeEntry != InvalidIdentifier, "Element entry is missing next free identifier!");

                // If there were more than a single element in the queue,
                // set the beginning of the queue to the next free element.
                m_freeListDequeue = elementEntry.nextFreeEntry;
                elementEntry.nextFreeEntry = InvalidIdentifier;
            }

            // Initialize element instance with a new copy.
            elementEntry.element = ElementType(std::forward<Arguments>(arguments)...);
            elementEntry.exists = true;

            // Increment the element count.
            m_elementCount++;

            // Return a handle to the created element.
            return HandleType(entryIndex + m_startingIdentifier, elementEntry.handleVersion);
        }

        // Returns an element with the handle.
        const ElementType* Lookup(const HandleType& handle) const
        {
            // Calculate the entry index.
            ValueType entryIndex = handle.identifier - m_startingIdentifier;

            if(entryIndex >= m_entries.size())
                return nullptr;

            // Retrieve the element entry.
            const ElementEntry& elementEntry = m_entries[entryIndex];

            if(elementEntry.handleVersion != handle.version)
                return nullptr;

            ASSERT(elementEntry.exists, "Received a valid handle to an entry that does not exist!");

            // Return the element pointer.
            return &elementEntry.element;
        }

        ElementType* Lookup(const HandleType& handle)
        {
            // Call the const function and then cast the const modifier away.
            return const_cast<ElementType*>(const_cast<const SlotArray<ElementType>&>(*this).Lookup(handle));
        }

        // Removes an element with the handle.
        bool Remove(const HandleType& handle)
        {
            // Calculate the entry index.
            ValueType entryIndex = handle.identifier - m_startingIdentifier;

            if(entryIndex >= m_entries.size())
                return false;

            // Retrieve the element entry.
            ElementEntry& elementEntry = m_entries[entryIndex];

            if(elementEntry.handleVersion != handle.version)
                return false;

            ASSERT(elementEntry.exists, "Received a valid handle to an entry that does not exist!");

            // Invalidate the element entry.
            elementEntry.element = ElementType();
            elementEntry.handleVersion += 1;
            elementEntry.exists = false;

            // Decrement the element count.
            m_elementCount--;

            // Add the element entry to the free list queue.
            if(m_freeListDequeue == InvalidIdentifier)
            {
                // If there are no elements in the queue,
                // set the element as the only one in the queue.
                m_freeListDequeue = entryIndex;
                m_freeListEnqueue = entryIndex;
            }
            else
            {
                // Retrieve last free handle entry on the free list.
                int lastFreeIndex = m_freeListEnqueue - m_startingIdentifier;
                ElementEntry& lastFreeEntry = m_entries[lastFreeIndex];

                // Make sure the last element in the queue does not point at another free element.
                ASSERT(elementEntry.nextFreeEntry == InvalidIdentifier);
                ASSERT(lastFreeEntry.nextFreeEntry == InvalidIdentifier);

                // If there are other elements in the queue,
                // add the element to the end of the queue chain.
                lastFreeEntry.nextFreeEntry = entryIndex;
                m_freeListEnqueue = entryIndex;
            }

            return true;
        }

        bool IsHandleValid(const HandleType& handle)
        {
            // Calculate the entry index.
            ValueType entryIndex = handle.identifier - m_startingIdentifier;

            if(entryIndex >= m_entries.size())
                return false;

            // Retrieve the element entry.
            ElementEntry& elementEntry = m_entries[entryIndex];

            if(elementEntry.handleVersion != handle.version)
                return false;

            ASSERT(elementEntry.exists, "Received a valid handle to an entry that does not exist!");

            // Handle seems valid.
            return true;
        }

        unsigned int GetSize() const
        {
            return m_elementCount;
        }

        Iterator Begin()
        {
            // Find first valid element.
            for(std::size_t i = 0; i < m_entries.size(); ++i)
            {
                if(m_entries[i].exists)
                {
                    return Iterator(this, i + m_startingIdentifier);
                }
            }

            // Return an invalid iterator otherwise.
            return Iterator();
        }

        Iterator End()
        {
            // Return an invalid iterator.
            return Iterator();
        }

    private:
        // Number of existing elements.
        unsigned int m_elementCount;

        // Current starting handle index.
        ValueType m_startingIdentifier;

        // List of all element entries.
        std::deque<ElementEntry> m_entries;

        // List of unused element entries.
        ValueType m_freeListDequeue;
        ValueType m_freeListEnqueue;
    };
}
