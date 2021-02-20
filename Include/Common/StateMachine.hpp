/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <memory>
#include "Common/Debug.hpp"

/*
    State Machine

    Stores current state and performs simple transitions between two possible states.
    Supports recursive transitions where next state can immediately transition to another.
    Reference to previous state is not discarded until after next state finishes entering.
    Works on states wrapped in shared pointers to accustom generic usage.

    See unit tests for example usage.
*/

namespace Common
{
    template<typename Type>
    class StateMachine;

    template<typename Type>
    class State
    {
    public:
        StateMachine<Type>* GetStateMachine() const
        {
            return m_stateMachine;
        }

        bool HasStateMachine() const
        {
            return m_stateMachine != nullptr;
        }

    protected:
        State() = default;
        virtual ~State() = default;

        virtual bool CanEnterState(Type* currentState) const
        {
            return true;
        }

        virtual bool CanExitState(Type* nextState) const
        {
            return true;
        }

        virtual void OnEnterState(Type* previousState)
        {
        }

        virtual void OnExitState(Type* nextState)
        {
        }

    private:
        friend StateMachine<Type>;

        void SetStateMachine(StateMachine<Type>* stateMachine)
        {
            if(stateMachine)
            {
                ASSERT(!m_stateMachine, "Assigning state machine reference twice!");
            }

            m_stateMachine = stateMachine;
        }

        StateMachine<Type>* m_stateMachine = nullptr;
    };

    template<typename Type>
    class StateMachine
    {
    public:
        static_assert(std::is_base_of<State<Type>, Type>::value, "Type is not derived from State<Type>!");
        using StateSharedPtr = std::shared_ptr<Type>;

        StateMachine() = default;
        ~StateMachine()
        {
            ChangeState(nullptr);
        }

        bool ChangeState(StateSharedPtr newState)
        {
            if(!CanExitCurrentState(newState))
                return false;

            if(!CanEnterNewState(newState))
                return false;

            TransitionStates(newState);

            return true;
        }

        const StateSharedPtr& GetState() const
        {
            return m_currentState;
        }

        bool HasState() const
        {
            return m_currentState != nullptr;
        }

    private:
        bool CanExitCurrentState(const StateSharedPtr& newState) const
        {
            if(m_currentState)
            {
                ASSERT(m_currentState->GetStateMachine() == this, "Current state does not have reference set to this state machine!");

                if(!m_currentState->CanExitState(newState.get()))
                {
                    return false;
                }
            }

            return true;
        }

        bool CanEnterNewState(const StateSharedPtr& newState) const
        {
            if(newState)
            {
                if(newState->HasStateMachine())
                {
                    LOG_WARNING("Attempted transition to state that is already in use!");
                    return false;
                }

                if(!newState->CanEnterState(m_currentState.get()))
                {
                    return false;
                }
            }

            return true;
        }

        void TransitionStates(StateSharedPtr& newState)
        {
            // Methods called on states are in very specific order to detect
            // illegal transitions occurring during exit and enter transitions.

            StateSharedPtr previousState;

            if(m_currentState)
            {
                previousState = m_currentState;
                previousState->SetStateMachine(nullptr);
                previousState->OnExitState(newState.get());

                ASSERT(m_currentState == previousState, "Illegal state transition occured during exit of current state!");
                m_currentState = nullptr;
            }

            if(newState)
            {
                m_currentState = newState;
                m_currentState->SetStateMachine(this);
                m_currentState->OnEnterState(previousState.get());
            }
        }

        StateSharedPtr m_currentState = nullptr;
    };
}
