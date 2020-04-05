/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include <string>
#include <Common/StateMachine.hpp>
#include "TestHelpers.hpp"

class TestState : public State<TestState>
{
public:
    TestState(const char* name = "", int counter = 0) :
        name(name), counter(counter)
    {
    }

    bool CanEnterState(TestState* previousState) const override
    {
        return allowEnter;
    }

    bool CanExitState(TestState* nextState) const override
    {
        return allowExit;
    }

    void OnEnterState(TestState* previousState) override
    {
        counter += 10;

        if(transitionOnEnter && GetStateMachine())
        {
            if(GetStateMachine()->ChangeState(transitionOnEnter))
                return;
        }
    }

    void OnExitState(TestState* nextState) override
    {
        counter -= 1;

        if(transitionOnExit && GetStateMachine())
        {
            if(GetStateMachine()->ChangeState(transitionOnExit))
                return;
        }
    }

public:
    std::string name;
    int counter;

    bool allowEnter = true;
    bool allowExit = true;

    std::shared_ptr<TestState> transitionOnEnter;
    std::shared_ptr<TestState> transitionOnExit;
};

bool TestTransitions()
{
    // Create state machine.
    StateMachine<TestState> stateMachine;

    TEST_FALSE(stateMachine.HasCurrentState());
    TEST_EQ(stateMachine.GetCurrentState(), nullptr);

    // Create state.
    auto stateOne = std::make_shared<TestState>("One", 100);

    TEST_FALSE(stateOne->HasStateMachine());
    TEST_EQ(stateOne->GetStateMachine(), nullptr);

    // Change to state.
    TEST_TRUE(stateMachine.ChangeState(stateOne));
    TEST_TRUE(stateMachine.HasCurrentState());
    TEST_EQ(stateMachine.GetCurrentState(), stateOne);

    TEST_TRUE(stateOne->HasStateMachine());
    TEST_EQ(stateOne->GetStateMachine(), &stateMachine);
    TEST_EQ(stateOne->counter, 110);

    // Change to another state.
    auto stateTwo = std::make_shared<TestState>("Two", 200);

    TEST_FALSE(stateTwo->HasStateMachine());
    TEST_EQ(stateTwo->GetStateMachine(), nullptr);

    TEST_TRUE(stateMachine.ChangeState(stateTwo));
    TEST_TRUE(stateMachine.HasCurrentState());
    TEST_EQ(stateMachine.GetCurrentState(), stateTwo);

    TEST_FALSE(stateOne->HasStateMachine());
    TEST_EQ(stateOne->GetStateMachine(), nullptr);
    TEST_EQ(stateOne->counter, 109);

    TEST_TRUE(stateTwo->HasStateMachine());
    TEST_EQ(stateTwo->GetStateMachine(), &stateMachine);
    TEST_EQ(stateTwo->counter, 210);

    // Change to current state.
    TEST_FALSE(stateMachine.ChangeState(stateTwo));
    TEST_TRUE(stateMachine.HasCurrentState());
    TEST_EQ(stateMachine.GetCurrentState(), stateTwo);

    TEST_FALSE(stateOne->HasStateMachine());
    TEST_EQ(stateOne->GetStateMachine(), nullptr);
    TEST_EQ(stateOne->counter, 109);

    TEST_TRUE(stateTwo->HasStateMachine());
    TEST_EQ(stateTwo->GetStateMachine(), &stateMachine);
    TEST_EQ(stateTwo->counter, 210);

    // Change to previous state.
    TEST_TRUE(stateMachine.ChangeState(stateOne));
    TEST_TRUE(stateMachine.HasCurrentState());
    TEST_EQ(stateMachine.GetCurrentState(), stateOne);

    TEST_TRUE(stateOne->HasStateMachine());
    TEST_EQ(stateOne->GetStateMachine(), &stateMachine);
    TEST_EQ(stateOne->counter, 119);

    TEST_FALSE(stateTwo->HasStateMachine());
    TEST_EQ(stateTwo->GetStateMachine(), nullptr);
    TEST_EQ(stateTwo->counter, 209);

    // Change to blocked enter transition.
    stateTwo->allowEnter = false;

    TEST_FALSE(stateMachine.ChangeState(stateTwo));
    TEST_TRUE(stateMachine.HasCurrentState());
    TEST_EQ(stateMachine.GetCurrentState(), stateOne);

    TEST_TRUE(stateOne->HasStateMachine());
    TEST_EQ(stateOne->GetStateMachine(), &stateMachine);
    TEST_EQ(stateOne->counter, 119);

    TEST_FALSE(stateTwo->HasStateMachine());
    TEST_EQ(stateTwo->GetStateMachine(), nullptr);
    TEST_EQ(stateTwo->counter, 209);

    stateTwo->allowEnter = true;

    // Change to blocked exit transition.
    stateOne->allowExit = false;

    TEST_FALSE(stateMachine.ChangeState(stateTwo));
    TEST_TRUE(stateMachine.HasCurrentState());
    TEST_EQ(stateMachine.GetCurrentState(), stateOne);

    TEST_TRUE(stateOne->HasStateMachine());
    TEST_EQ(stateOne->GetStateMachine(), &stateMachine);
    TEST_EQ(stateOne->counter, 119);

    TEST_FALSE(stateTwo->HasStateMachine());
    TEST_EQ(stateTwo->GetStateMachine(), nullptr);
    TEST_EQ(stateTwo->counter, 209);

    stateOne->allowExit = true;

    // Check state after freeing shared pointers.
    stateOne = nullptr;
    stateTwo = nullptr;

    TEST_TRUE(stateMachine.HasCurrentState());
    stateOne = stateMachine.GetCurrentState();

    TEST_TRUE(stateOne->HasStateMachine());
    TEST_EQ(stateOne->GetStateMachine(), &stateMachine);
    TEST_EQ(stateOne->name, "One");
    TEST_EQ(stateOne->counter, 119);

    // Discard current state.
    stateMachine.ChangeState(nullptr);

    TEST_FALSE(stateMachine.HasCurrentState());
    TEST_EQ(stateMachine.GetCurrentState(), nullptr);

    TEST_FALSE(stateOne->HasStateMachine());
    TEST_EQ(stateOne->GetStateMachine(), nullptr);
    TEST_EQ(stateOne->counter, 118);

    return true;
}

bool TestRecursiveTransitions()
{
    // Create state machine.
    StateMachine<TestState> stateMachine;

    TEST_FALSE(stateMachine.HasCurrentState());
    TEST_EQ(stateMachine.GetCurrentState(), nullptr);

    // Setup transition chain.
    auto stateFirst = std::make_shared<TestState>("First", 100);

    TEST_FALSE(stateFirst->HasStateMachine());
    TEST_EQ(stateFirst->GetStateMachine(), nullptr);

    auto stateSecond = std::make_shared<TestState>("Second", 200);

    TEST_FALSE(stateSecond->HasStateMachine());
    TEST_EQ(stateSecond->GetStateMachine(), nullptr);

    auto stateThird = std::make_shared<TestState>("Third", 300);

    TEST_FALSE(stateThird->HasStateMachine());
    TEST_EQ(stateThird->GetStateMachine(), nullptr);

    stateFirst->transitionOnEnter = stateSecond;
    stateSecond->transitionOnEnter = stateThird;
    stateThird->allowEnter = false;

    // Perform chained transitions.
    TEST_TRUE(stateMachine.ChangeState(stateFirst));
    TEST_TRUE(stateMachine.HasCurrentState());
    TEST_EQ(stateMachine.GetCurrentState(), stateSecond);

    TEST_FALSE(stateFirst->HasStateMachine());
    TEST_EQ(stateFirst->GetStateMachine(), nullptr);
    TEST_EQ(stateFirst->counter, 109);

    TEST_TRUE(stateSecond->HasStateMachine());
    TEST_EQ(stateSecond->GetStateMachine(), &stateMachine);
    TEST_EQ(stateSecond->counter, 210);

    TEST_FALSE(stateThird->HasStateMachine());
    TEST_EQ(stateThird->GetStateMachine(), nullptr);
    TEST_EQ(stateThird->counter, 300);

    return true;
}

int TestExitTransition()
{
    // Create state machine.
    StateMachine<TestState> stateMachine;

    TEST_FALSE(stateMachine.HasCurrentState());
    TEST_EQ(stateMachine.GetCurrentState(), nullptr);

    // Create states.
    auto stateFirst = std::make_shared<TestState>();
    auto stateSecond = std::make_shared<TestState>();
    stateFirst->transitionOnExit = stateSecond;

    TEST_FALSE(stateFirst->HasStateMachine());
    TEST_EQ(stateFirst->GetStateMachine(), nullptr);

    TEST_FALSE(stateSecond->HasStateMachine());
    TEST_EQ(stateSecond->GetStateMachine(), nullptr);

    // Trigger transition on exit.
    TEST_TRUE(stateMachine.ChangeState(stateFirst));
    TEST_TRUE(stateMachine.HasCurrentState());
    TEST_EQ(stateMachine.GetCurrentState(), stateFirst);

    TEST_TRUE(stateMachine.ChangeState(nullptr));
    TEST_FALSE(stateMachine.HasCurrentState());
    TEST_EQ(stateMachine.GetCurrentState(), nullptr);

    return true;
}

int TestCleanup()
{
    // Create state.
    auto state = std::make_shared<TestState>();

    {
        // Create state machine.
        StateMachine<TestState> stateMachine;

        TEST_FALSE(stateMachine.HasCurrentState());
        TEST_EQ(stateMachine.GetCurrentState(), nullptr);

        // Transition to state
        TEST_TRUE(stateMachine.ChangeState(state));
        TEST_TRUE(stateMachine.HasCurrentState());
        TEST_EQ(stateMachine.GetCurrentState(), state);

        TEST_TRUE(state->HasStateMachine());
        TEST_EQ(state->GetStateMachine(), &stateMachine);
        TEST_EQ(state->counter, 10);
    }

    // Check if state exit properly.
    TEST_FALSE(state->HasStateMachine());
    TEST_EQ(state->GetStateMachine(), nullptr);
    TEST_EQ(state->counter, 9);
    
    return true;
}

int main()
{
    if(!TestTransitions())
        return 1;

    if(!TestRecursiveTransitions())
        return 1;

    if(!TestExitTransition())
        return 1;

    if(!TestCleanup())
        return 1;

    return 0;
}
