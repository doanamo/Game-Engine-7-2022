/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Debug.hpp"
#include "NonCopyable.hpp"

/*
    Linked List

    Circular linked list node which can be interconnected to each other in
    arbitrary ways with implementation that is quick and favors simplicity.
*/

namespace Common
{
    template<typename Type>
    class ListNode : private NonCopyable
    {
    public:
        ListNode() :
            m_reference(nullptr),
            m_previous(this),
            m_next(this)
        {
        }

        ListNode(ListNode<Type>&& other) :
            ListNode()
        {
            // Call move assignment.
            *this = std::move(other);
        }

        ListNode<Type>& operator=(ListNode<Type>&& other)
        {
            // Swap class members.
            // Do not swap reference to object.
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

        void SetReference(Type* reference)
        {
            m_reference = reference;
        }

        Type* GetReference() const
        {
            return m_reference;
        }

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

        ListNode<Type>* GetPrevious() const
        {
            return m_previous;
        }

        ListNode<Type>* GetNext() const
        {
            return m_next;
        }

        bool IsFree() const
        {
            return m_previous == this;
        }

    private:
        // Strong object reference.
        // Will not be swapped on move.
        Type* m_reference;

        // Linked node references.
        ListNode<Type>* m_previous;
        ListNode<Type>* m_next;
    };
}
