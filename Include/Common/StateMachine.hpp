/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <memory>
#include "Debug.hpp"

/*
    State Machine

    Stores current state and performs simple transitions between two possible states.
    Supports recursive transitions where next state can immediately transition to another.
    Reference to previous state is not discarded until after next state finishes entering.
    Works on states wrapped in shared pointers to accustom generic usage.

    class ExampleState : public State<ExampleState>
    {
    public:
        bool CanEnterState(ExampleState* currentState) const override
        {
            return true;
        }

        bool CanExitState(ExampleState* nextState) const override
        {
            return true;
        }

        void OnEnterState(ExampleState* previousState) override
        {
        }

        void OnExitState(ExampleState* nextState) override
        {
        }
    };

    void ExampleUsage()
    {
        StateMachine<ExampleState> stateMachine;
        stateMachine.ChangeState(std::make_shared<ExampleState>());
    }
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

    public:
        StateMachine() = default;
        ~StateMachine()
        {
            // Explicitly transition out of current
            // state so OnExitState() is called.
            ChangeState(nullptr);
        }

        bool ChangeState(StateSharedPtr newState)
        {
            // Check if we can exit current state.
            if(m_currentState)
            {
                ASSERT(m_currentState->GetStateMachine() == this, "Current state does not have reference set to this state machine!");

                if(!m_currentState->CanExitState(newState.get()))
                {
                    return false;
                }
            }

            // Check if we can enter new state.
            if(newState)
            {
                if(newState->HasStateMachine())
                {
                    LOG_WARNING("State machine stopped transition to state that is already in use!");
                    return false;
                }

                if(!newState->CanEnterState(m_currentState.get()))
                {
                    return false;
                }
            }

            // Exit current state.
            StateSharedPtr previousState;

            if(m_currentState)
            {
                // Specific call order to disallow changing state on exit.
                previousState = m_currentState;
                previousState->SetStateMachine(nullptr);
                previousState->OnExitState(newState.get());

                ASSERT(m_currentState == previousState, "Illegal state transition occured during exit of current state!");
                m_currentState = nullptr;
            }

            // Enter new state.
            if(newState)
            {
                // Specific call order to allow changing state on enter.
                m_currentState = newState;
                m_currentState->SetStateMachine(this);
                m_currentState->OnEnterState(previousState.get());
            }

            // Success!
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
        StateSharedPtr m_currentState = nullptr;
    };
}
