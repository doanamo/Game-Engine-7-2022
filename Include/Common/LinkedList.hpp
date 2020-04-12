/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Debug.hpp"

/*
    Linked List

    Circular linked list node which can be interconnected to each other in
    arbitrary ways with implementation that is quick and favors simplicity.
*/

namespace Common
{
    // List node class.
    template<typename Type>
    class ListNode
    {
    public:
        // Constructor.
        ListNode() :
            m_reference(nullptr),
            m_previous(this),
            m_next(this)
        {
        }

        // Disallow copying.
        ListNode(const ListNode<Type>& other) = delete;
        ListNode<Type>& operator=(const ListNode<Type>& other) = delete;

        // Move constructor and assignment.
        ListNode(ListNode<Type>&& other) :
            ListNode()
        {
            // Call the move assignment.
            *this = std::move(other);
        }

        ListNode<Type>& operator=(ListNode<Type>&& other)
        {
            // Swap class members.
            // Do not swap the reference to the object.
            // Fix up swapped pointers along the way.
            // Order of instructions is very important here
            // due to deep dereferencing after pointer swap.
            std::swap(m_previous, other.m_previous);
            m_previous->m_next = this;
            other.m_previous->m_next = &other;

            std::swap(m_next, other.m_next);
            m_next->m_previous = this;
            other.m_next->m_previous = &other;

            return *this;
        }

        // Sets an object reference.
        void SetReference(Type* reference)
        {
            m_reference = reference;
        }

        // Gets an object reference.
        Type* GetReference() const
        {
            return m_reference;
        }

        // Insert node before another node.
        bool InsertBefore(ListNode<Type>* other)
        {
            // Check if other node is null.
            if(other == nullptr)
                return false;

            // Check if our node is free.
            if(m_next != this)
            {
                ASSERT(m_previous != this, "Previous node pointer should not be pointing at this!");
                return false;
            }

            ASSERT(m_previous == this, "Previous node pointer should be pointing at this!");

            // Insert node before another node.
            m_next = other;
            m_previous = other->m_previous;

            m_next->m_previous = this;
            m_previous->m_next = this;

            return true;
        }

        // Inserts node after another node.
        bool InsertAfter(ListNode<Type>* other)
        {
            // Check if other node is null.
            if(other == nullptr)
                return false;

            // Check if our node is free.
            if(m_previous != this)
            {
                ASSERT(m_next != this, "Next node pointer should not be pointing at this!");
                return false;
            }

            ASSERT(m_next == this, "Next node pointer should be pointing at this!");

            // Insert node after another node.
            m_previous = other;
            m_next = other->m_next;

            m_previous->m_next = this;
            m_next->m_previous = this;

            return true;
        }

        // Removes node from a current list.
        void Remove()
        {
            // Check if node is free.
            if(m_previous == this)
            {
                ASSERT(m_next == this, "Next node pointer should be pointing at this!");
                return;
            }

            // Remove node from a list.
            m_previous->m_next = m_next;
            m_next->m_previous = m_previous;

            m_previous = this;
            m_next = this;
        }

        // Gets the previous list node.
        ListNode<Type>* GetPrevious() const
        {
            return m_previous;
        }

        // Gets the next list node.
        ListNode<Type>* GetNext() const
        {
            return m_next;
        }

        // Checks if node is free.
        bool IsFree() const
        {
            return m_previous == this;
        }

    private:
        // Strong object reference.
        // Will not be swapped on move.
        Type* m_reference;

        // Node linked references.
        ListNode<Type>* m_previous;
        ListNode<Type>* m_next;
    };
}
