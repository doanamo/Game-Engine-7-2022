/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <memory>
#include "Common/Debug.hpp"

/*
    State Machine

    Stores current state and performs simple transitions between two possible states.
    Works on states wrapped in shared pointers to accustom generic usage.

    class ExampleState : public State<ExampleState>
    {
    public:
        bool CanEnterState(ExampleState* previousState) const override
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

template<typename Type>
class StateMachine;

template<typename Type>
class State
{
private:
    // Friend declaration.
    friend StateMachine<Type>;

protected:
    // Constructor and destructor.
    State() = default;
    virtual ~State() = default;

public:
    // Returns assigned state machine.
    StateMachine<Type>* GetStateMachine() const
    {
        return m_stateMachine;
    }

    // Checks if state has state machine assigned.
    bool HasStateMachine() const
    {
        return m_stateMachine != nullptr;
    }

private:
    // Assigns state machine owner.
    void SetStateMachine(StateMachine<Type>* stateMachine)
    {
        if(stateMachine)
        {
            ASSERT(!m_stateMachine, "Assigning state machine reference twice!");
        }
        
        m_stateMachine = stateMachine;
    }

protected:
    // Checks whether we can enter this state.
    // Argument represents current state that will be removed.
    virtual bool CanEnterState(Type* previousState) const
    {
        return true;
    }
    
    // Checks whether we can exit this state.
    // Argument represents next state waiting for transition.
    virtual bool CanExitState(Type* nextState) const
    {
        return true;
    }

    // Called when state machine transitions to this state.
    // Argument represents previous state that has been removed.
    virtual void OnEnterState(Type* previousState)
    {
    }

    // Called when state machine transitions out this state.
    // Argument represents next state waiting for transition.
    virtual void OnExitState(Type* nextState)
    {
    }

private:
    // Current state machine assigned.
    StateMachine<Type>* m_stateMachine = nullptr;
};

template<typename Type>
class StateMachine
{
public:
    // Check underlying type assumption.
    static_assert(std::is_base_of<State<Type>, Type>::value, "Type is not derived from State<Type>!");

    // Type declarations.
    using StateSharedPtr = std::shared_ptr<Type>;

public:
    // Constructor and destructor.
    StateMachine() = default;
    ~StateMachine()
    {
        // Explicitly transition out current state so OnExitState() is called.
        ChangeState(nullptr);
    }

    // Changes current state.
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
            // Specific order to disallow changing state on exit.
            previousState = m_currentState;
            previousState->SetStateMachine(nullptr);
            previousState->OnExitState(newState.get());
            
            ASSERT(m_currentState == previousState, "Illegal state transition occured during exit of current state!");
            m_currentState = nullptr;
        }

        // Enter new state.
        if(newState)
        {
            // Specific order to allow changing state on enter.
            m_currentState = newState;
            m_currentState->SetStateMachine(this);
            m_currentState->OnEnterState(previousState.get());
        }

        // Success!
        return true;
    }

    // Returns shared pointer to current state.
    const StateSharedPtr& GetCurrentState() const
    {
        return m_currentState;
    }

    // Returns whether current state exists.
    bool HasCurrentState() const
    {
        return m_currentState != nullptr;
    }

private:
    // Shared pointer to current state.
    StateSharedPtr m_currentState = nullptr;
};
