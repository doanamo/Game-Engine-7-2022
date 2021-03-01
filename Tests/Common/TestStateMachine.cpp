/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <string>
#include <doctest/doctest.h>
#include <Common/StateMachine.hpp>

class TestState : public Common::State<TestState>
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

TEST_CASE("State Machine")
{
    Common::StateMachine<TestState> stateMachine;
    CHECK_FALSE(stateMachine.HasState());
    CHECK_EQ(stateMachine.GetState(), nullptr);

    SUBCASE("Regular Transitions")
    {
        // Create states.
        int expectedCounterOne = 0;
        auto stateOne = std::make_shared<TestState>("One", 100);
        CHECK_FALSE(stateOne->HasStateMachine());
        CHECK_EQ(stateOne->GetStateMachine(), nullptr);
        CHECK_EQ(stateOne->counter, expectedCounterOne += 100);

        int expectedCounterTwo = 0;
        auto stateTwo = std::make_shared<TestState>("Two", 200);
        CHECK_FALSE(stateTwo->HasStateMachine());
        CHECK_EQ(stateTwo->GetStateMachine(), nullptr);
        CHECK_EQ(stateTwo->counter, expectedCounterTwo += 200);

        // Change to state one.
        CHECK(stateMachine.ChangeState(stateOne));
        CHECK(stateMachine.HasState());
        CHECK_EQ(stateMachine.GetState(), stateOne);

        CHECK(stateOne->HasStateMachine());
        CHECK_EQ(stateOne->GetStateMachine(), &stateMachine);
        CHECK_EQ(stateOne->counter, expectedCounterOne += 10);

        // Change current state.
        CHECK(stateMachine.ChangeState(stateTwo));
        CHECK(stateMachine.HasState());
        CHECK_EQ(stateMachine.GetState(), stateTwo);

        CHECK_FALSE(stateOne->HasStateMachine());
        CHECK_EQ(stateOne->GetStateMachine(), nullptr);
        CHECK_EQ(stateOne->counter, expectedCounterOne -= 1);

        CHECK(stateTwo->HasStateMachine());
        CHECK_EQ(stateTwo->GetStateMachine(), &stateMachine);
        CHECK_EQ(stateTwo->counter, expectedCounterTwo += 10);

        // Change to previous state.
        CHECK(stateMachine.ChangeState(stateOne));
        CHECK(stateMachine.HasState());
        CHECK_EQ(stateMachine.GetState(), stateOne);

        CHECK(stateOne->HasStateMachine());
        CHECK_EQ(stateOne->GetStateMachine(), &stateMachine);
        CHECK_EQ(stateOne->counter, expectedCounterOne += 10);

        CHECK_FALSE(stateTwo->HasStateMachine());
        CHECK_EQ(stateTwo->GetStateMachine(), nullptr);
        CHECK_EQ(stateTwo->counter, expectedCounterTwo -= 1);

        // Change to blocked enter transition.
        stateTwo->allowEnter = false;

        CHECK_FALSE(stateMachine.ChangeState(stateTwo));
        CHECK(stateMachine.HasState());
        CHECK_EQ(stateMachine.GetState(), stateOne);

        CHECK(stateOne->HasStateMachine());
        CHECK_EQ(stateOne->GetStateMachine(), &stateMachine);
        CHECK_EQ(stateOne->counter, expectedCounterOne);

        CHECK_FALSE(stateTwo->HasStateMachine());
        CHECK_EQ(stateTwo->GetStateMachine(), nullptr);
        CHECK_EQ(stateTwo->counter, expectedCounterTwo);

        stateTwo->allowEnter = true;

        // Change to blocked exit transition.
        stateOne->allowExit = false;

        CHECK_FALSE(stateMachine.ChangeState(stateTwo));
        CHECK(stateMachine.HasState());
        CHECK_EQ(stateMachine.GetState(), stateOne);

        CHECK(stateOne->HasStateMachine());
        CHECK_EQ(stateOne->GetStateMachine(), &stateMachine);
        CHECK_EQ(stateOne->counter, expectedCounterOne);

        CHECK_FALSE(stateTwo->HasStateMachine());
        CHECK_EQ(stateTwo->GetStateMachine(), nullptr);
        CHECK_EQ(stateTwo->counter, expectedCounterTwo);

        stateOne->allowExit = true;

        // Check state after freeing shared pointers.
        stateOne = nullptr;
        stateTwo = nullptr;

        CHECK(stateMachine.HasState());
        stateOne = stateMachine.GetState();

        CHECK(stateOne->HasStateMachine());
        CHECK_EQ(stateOne->GetStateMachine(), &stateMachine);
        CHECK_EQ(stateOne->name, "One");
        CHECK_EQ(stateOne->counter, expectedCounterOne);

        // Discard current state.
        stateMachine.ChangeState(nullptr);
        CHECK_FALSE(stateMachine.HasState());
        CHECK_EQ(stateMachine.GetState(), nullptr);

        CHECK_FALSE(stateOne->HasStateMachine());
        CHECK_EQ(stateOne->GetStateMachine(), nullptr);
        CHECK_EQ(stateOne->counter, expectedCounterOne -= 1);
    }

    SUBCASE("Recursive Transitions")
    {
        // Create state machine.
        Common::StateMachine<TestState> stateMachine;
        CHECK_FALSE(stateMachine.HasState());
        CHECK_EQ(stateMachine.GetState(), nullptr);

        // Setup transition chain.
        auto stateFirst = std::make_shared<TestState>("First", 100);
        CHECK_FALSE(stateFirst->HasStateMachine());
        CHECK_EQ(stateFirst->GetStateMachine(), nullptr);

        auto stateSecond = std::make_shared<TestState>("Second", 200);
        CHECK_FALSE(stateSecond->HasStateMachine());
        CHECK_EQ(stateSecond->GetStateMachine(), nullptr);

        auto stateThird = std::make_shared<TestState>("Third", 300);
        CHECK_FALSE(stateThird->HasStateMachine());
        CHECK_EQ(stateThird->GetStateMachine(), nullptr);

        stateFirst->transitionOnEnter = stateSecond;
        stateSecond->transitionOnEnter = stateThird;
        stateThird->allowEnter = false;

        // Perform chained transitions.
        CHECK(stateMachine.ChangeState(stateFirst));
        CHECK(stateMachine.HasState());
        CHECK_EQ(stateMachine.GetState(), stateSecond);

        CHECK_FALSE(stateFirst->HasStateMachine());
        CHECK_EQ(stateFirst->GetStateMachine(), nullptr);
        CHECK_EQ(stateFirst->counter, 109);

        CHECK(stateSecond->HasStateMachine());
        CHECK_EQ(stateSecond->GetStateMachine(), &stateMachine);
        CHECK_EQ(stateSecond->counter, 210);

        CHECK_FALSE(stateThird->HasStateMachine());
        CHECK_EQ(stateThird->GetStateMachine(), nullptr);
        CHECK_EQ(stateThird->counter, 300);
    }

    SUBCASE("Exit Transitions")
    {
        // Create state machine.
        Common::StateMachine<TestState> stateMachine;
        CHECK_FALSE(stateMachine.HasState());
        CHECK_EQ(stateMachine.GetState(), nullptr);

        // Create states.
        auto stateFirst = std::make_shared<TestState>();
        auto stateSecond = std::make_shared<TestState>();
        stateFirst->transitionOnExit = stateSecond;

        CHECK_FALSE(stateFirst->HasStateMachine());
        CHECK_EQ(stateFirst->GetStateMachine(), nullptr);

        CHECK_FALSE(stateSecond->HasStateMachine());
        CHECK_EQ(stateSecond->GetStateMachine(), nullptr);

        // Trigger transition on exit.
        CHECK(stateMachine.ChangeState(stateFirst));
        CHECK(stateMachine.HasState());
        CHECK_EQ(stateMachine.GetState(), stateFirst);

        CHECK(stateMachine.ChangeState(nullptr));
        CHECK_FALSE(stateMachine.HasState());
        CHECK_EQ(stateMachine.GetState(), nullptr);
    }

    CHECK(stateMachine.ChangeState(nullptr));
    CHECK_FALSE(stateMachine.HasState());
    CHECK_EQ(stateMachine.GetState(), nullptr);
}
