/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Common/Debug.hpp"
#include "Common/NonCopyable.hpp"

/*
    Linked List

    Circular double linked list implementation that is quick and favors simplicity. Nodes never
    point at null previous/next node and a list with single element is represented by single node.
    Having list with single node means the only node has its previous/next nodes pointing at itself
    (circular property). List being circular does not mean that it can't be used as finite sequence.

    Since linked nodes are meant to be stored in an intrusive way along with objects, reference can
    be set to point back at object that given node refers to.
*/

namespace Common
{
    template<typename Type>
    class ListNode : private Common::NonCopyable
    {
    public:
        ListNode(Type* owner = nullptr) :
            m_reference(owner),
            m_previous(this),
            m_next(this)
        {
        }

        ListNode(ListNode<Type>&& other) :
            ListNode()
        {
            *this = std::move(other);
        }

        ListNode<Type>& operator=(ListNode<Type>&& other)
        {
            /*
                Do not swap references to objects as we will be pointing at same addresses 
                memory, as only data of both objects is moved. Fix up swapped pointers along
                the way for which order of instructions is very important here due to deep
                dereferencing after pointer swap.
            */

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

        bool HasReference() const
        {
            return m_reference != nullptr;
        }

        template<typename Function, typename ...Arguments>
        bool ForEach(Function function, Arguments&&... arguments)
        {
            /*
                Iterate over all linked receivers (excluding ourselves) in an ordered fashion.
                This handles adding/removing current and next nodes that are or will be processed,
                but will not process nodes added before currently processed node.

                Cache previous and next iterators to determine cases where currently invoked
                function is adding or removing nodes during its invocation.

                There are three cases to consider:
                1. Current node has been removed.
                   Current iterator is invalid, we use cached next iterator.
                2. New node has been inserted.
                   Cached next iterator may be invalid, we acquire next iterator again.
                3. Current node has been removed and new node has been inserted.
                   Current iterator is invalid and next iterator may be invalid,
                   we need to acquire next iterator from cached previous iterator.
            */

            ListNode<Type>* iterator = this->GetNext();

            while(iterator != this)
            {
                ASSERT(iterator != nullptr);

                ListNode<Type>* previousIterator = iterator->GetPrevious();
                ListNode<Type>* nextIterator = iterator->GetNext();

                if(!function(*iterator, std::forward<Arguments>(arguments)...))
                    return false;

                if(nextIterator == this)
                {
                    if(iterator->IsFree())
                    {
                        // Case 3: Acquire next iterator from cached previous iterator.
                        iterator = previousIterator->GetNext();
                    }
                    else
                    {
                        // Case 2: Acquire next iterator again.
                        iterator = iterator->GetNext();
                    }
                }
                else
                {
                    // Case 1: Use cached next iterator.
                    iterator = nextIterator;
                }
            }

            return true;
        }

        bool InsertBefore(ListNode<Type>* other)
        {
            if(IsFree() && other != nullptr)
            {
                m_next = other;
                m_previous = other->m_previous;

                m_next->m_previous = this;
                m_previous->m_next = this;

                return true;
            }

            return false;
        }

        bool InsertAfter(ListNode<Type>* other)
        {
            if(IsFree() && other != nullptr)
            {
                m_previous = other;
                m_next = other->m_next;

                m_previous->m_next = this;
                m_next->m_previous = this;

                return true;
            }

            return false;
        }

        void Remove()
        {
            if(!IsFree())
            {
                m_previous->m_next = m_next;
                m_next->m_previous = m_previous;

                m_previous = this;
                m_next = this;
            }
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
            if(m_previous == this)
            {
                ASSERT(m_next == this);
                return true;
            }
            else
            {
                ASSERT(m_next != this);
                return false;
            }
        }

    private:
        Type* m_reference;
        ListNode<Type>* m_previous;
        ListNode<Type>* m_next;
    };
}
