/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#define DOCTEST_CONFIG_NO_SHORT_MACRO_NAMES
#include <doctest/doctest.h>

#include <string>
#include <memory>
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

DOCTEST_TEST_CASE("State Machine")
{
    Common::StateMachine<TestState> stateMachine;
    DOCTEST_CHECK_FALSE(stateMachine.HasState());
    DOCTEST_CHECK_EQ(stateMachine.GetState(), nullptr);

    DOCTEST_SUBCASE("Regular transitions")
    {
        // Create states.
        int expectedCounterOne = 0;
        auto stateOne = std::make_shared<TestState>("One", 100);
        DOCTEST_CHECK_FALSE(stateOne->HasStateMachine());
        DOCTEST_CHECK_EQ(stateOne->GetStateMachine(), nullptr);
        DOCTEST_CHECK_EQ(stateOne->counter, expectedCounterOne += 100);

        int expectedCounterTwo = 0;
        auto stateTwo = std::make_shared<TestState>("Two", 200);
        DOCTEST_CHECK_FALSE(stateTwo->HasStateMachine());
        DOCTEST_CHECK_EQ(stateTwo->GetStateMachine(), nullptr);
        DOCTEST_CHECK_EQ(stateTwo->counter, expectedCounterTwo += 200);

        // Change to state one.
        DOCTEST_CHECK(stateMachine.ChangeState(stateOne));
        DOCTEST_CHECK(stateMachine.HasState());
        DOCTEST_CHECK_EQ(stateMachine.GetState(), stateOne);

        DOCTEST_CHECK(stateOne->HasStateMachine());
        DOCTEST_CHECK_EQ(stateOne->GetStateMachine(), &stateMachine);
        DOCTEST_CHECK_EQ(stateOne->counter, expectedCounterOne += 10);

        // Change current state.
        DOCTEST_CHECK(stateMachine.ChangeState(stateTwo));
        DOCTEST_CHECK(stateMachine.HasState());
        DOCTEST_CHECK_EQ(stateMachine.GetState(), stateTwo);

        DOCTEST_CHECK_FALSE(stateOne->HasStateMachine());
        DOCTEST_CHECK_EQ(stateOne->GetStateMachine(), nullptr);
        DOCTEST_CHECK_EQ(stateOne->counter, expectedCounterOne -= 1);

        DOCTEST_CHECK(stateTwo->HasStateMachine());
        DOCTEST_CHECK_EQ(stateTwo->GetStateMachine(), &stateMachine);
        DOCTEST_CHECK_EQ(stateTwo->counter, expectedCounterTwo += 10);

        // Change to previous state.
        DOCTEST_CHECK(stateMachine.ChangeState(stateOne));
        DOCTEST_CHECK(stateMachine.HasState());
        DOCTEST_CHECK_EQ(stateMachine.GetState(), stateOne);

        DOCTEST_CHECK(stateOne->HasStateMachine());
        DOCTEST_CHECK_EQ(stateOne->GetStateMachine(), &stateMachine);
        DOCTEST_CHECK_EQ(stateOne->counter, expectedCounterOne += 10);

        DOCTEST_CHECK_FALSE(stateTwo->HasStateMachine());
        DOCTEST_CHECK_EQ(stateTwo->GetStateMachine(), nullptr);
        DOCTEST_CHECK_EQ(stateTwo->counter, expectedCounterTwo -= 1);

        // Change to blocked enter transition.
        stateTwo->allowEnter = false;

        DOCTEST_CHECK_FALSE(stateMachine.ChangeState(stateTwo));
        DOCTEST_CHECK(stateMachine.HasState());
        DOCTEST_CHECK_EQ(stateMachine.GetState(), stateOne);

        DOCTEST_CHECK(stateOne->HasStateMachine());
        DOCTEST_CHECK_EQ(stateOne->GetStateMachine(), &stateMachine);
        DOCTEST_CHECK_EQ(stateOne->counter, expectedCounterOne);

        DOCTEST_CHECK_FALSE(stateTwo->HasStateMachine());
        DOCTEST_CHECK_EQ(stateTwo->GetStateMachine(), nullptr);
        DOCTEST_CHECK_EQ(stateTwo->counter, expectedCounterTwo);

        stateTwo->allowEnter = true;

        // Change to blocked exit transition.
        stateOne->allowExit = false;

        DOCTEST_CHECK_FALSE(stateMachine.ChangeState(stateTwo));
        DOCTEST_CHECK(stateMachine.HasState());
        DOCTEST_CHECK_EQ(stateMachine.GetState(), stateOne);

        DOCTEST_CHECK(stateOne->HasStateMachine());
        DOCTEST_CHECK_EQ(stateOne->GetStateMachine(), &stateMachine);
        DOCTEST_CHECK_EQ(stateOne->counter, expectedCounterOne);

        DOCTEST_CHECK_FALSE(stateTwo->HasStateMachine());
        DOCTEST_CHECK_EQ(stateTwo->GetStateMachine(), nullptr);
        DOCTEST_CHECK_EQ(stateTwo->counter, expectedCounterTwo);

        stateOne->allowExit = true;

        // DOCTEST_CHECK state after freeing shared pointers.
        stateOne = nullptr;
        stateTwo = nullptr;

        DOCTEST_CHECK(stateMachine.HasState());
        stateOne = stateMachine.GetState();

        DOCTEST_CHECK(stateOne->HasStateMachine());
        DOCTEST_CHECK_EQ(stateOne->GetStateMachine(), &stateMachine);
        DOCTEST_CHECK_EQ(stateOne->name, "One");
        DOCTEST_CHECK_EQ(stateOne->counter, expectedCounterOne);

        // Discard current state.
        stateMachine.ChangeState(nullptr);
        DOCTEST_CHECK_FALSE(stateMachine.HasState());
        DOCTEST_CHECK_EQ(stateMachine.GetState(), nullptr);

        DOCTEST_CHECK_FALSE(stateOne->HasStateMachine());
        DOCTEST_CHECK_EQ(stateOne->GetStateMachine(), nullptr);
        DOCTEST_CHECK_EQ(stateOne->counter, expectedCounterOne -= 1);
    }

    DOCTEST_SUBCASE("Recursive transitions")
    {
        // Create state machine.
        Common::StateMachine<TestState> stateMachine;
        DOCTEST_CHECK_FALSE(stateMachine.HasState());
        DOCTEST_CHECK_EQ(stateMachine.GetState(), nullptr);

        // Setup transition chain.
        auto stateFirst = std::make_shared<TestState>("First", 100);
        DOCTEST_CHECK_FALSE(stateFirst->HasStateMachine());
        DOCTEST_CHECK_EQ(stateFirst->GetStateMachine(), nullptr);

        auto stateSecond = std::make_shared<TestState>("Second", 200);
        DOCTEST_CHECK_FALSE(stateSecond->HasStateMachine());
        DOCTEST_CHECK_EQ(stateSecond->GetStateMachine(), nullptr);

        auto stateThird = std::make_shared<TestState>("Third", 300);
        DOCTEST_CHECK_FALSE(stateThird->HasStateMachine());
        DOCTEST_CHECK_EQ(stateThird->GetStateMachine(), nullptr);

        stateFirst->transitionOnEnter = stateSecond;
        stateSecond->transitionOnEnter = stateThird;
        stateThird->allowEnter = false;

        // Perform chained transitions.
        DOCTEST_CHECK(stateMachine.ChangeState(stateFirst));
        DOCTEST_CHECK(stateMachine.HasState());
        DOCTEST_CHECK_EQ(stateMachine.GetState(), stateSecond);

        DOCTEST_CHECK_FALSE(stateFirst->HasStateMachine());
        DOCTEST_CHECK_EQ(stateFirst->GetStateMachine(), nullptr);
        DOCTEST_CHECK_EQ(stateFirst->counter, 109);

        DOCTEST_CHECK(stateSecond->HasStateMachine());
        DOCTEST_CHECK_EQ(stateSecond->GetStateMachine(), &stateMachine);
        DOCTEST_CHECK_EQ(stateSecond->counter, 210);

        DOCTEST_CHECK_FALSE(stateThird->HasStateMachine());
        DOCTEST_CHECK_EQ(stateThird->GetStateMachine(), nullptr);
        DOCTEST_CHECK_EQ(stateThird->counter, 300);
    }

    DOCTEST_SUBCASE("Exit transitions")
    {
        // Create state machine.
        Common::StateMachine<TestState> stateMachine;
        DOCTEST_CHECK_FALSE(stateMachine.HasState());
        DOCTEST_CHECK_EQ(stateMachine.GetState(), nullptr);

        // Create states.
        auto stateFirst = std::make_shared<TestState>();
        auto stateSecond = std::make_shared<TestState>();
        stateFirst->transitionOnExit = stateSecond;

        DOCTEST_CHECK_FALSE(stateFirst->HasStateMachine());
        DOCTEST_CHECK_EQ(stateFirst->GetStateMachine(), nullptr);

        DOCTEST_CHECK_FALSE(stateSecond->HasStateMachine());
        DOCTEST_CHECK_EQ(stateSecond->GetStateMachine(), nullptr);

        // Trigger transition on exit.
        DOCTEST_CHECK(stateMachine.ChangeState(stateFirst));
        DOCTEST_CHECK(stateMachine.HasState());
        DOCTEST_CHECK_EQ(stateMachine.GetState(), stateFirst);

        DOCTEST_CHECK(stateMachine.ChangeState(nullptr));
        DOCTEST_CHECK_FALSE(stateMachine.HasState());
        DOCTEST_CHECK_EQ(stateMachine.GetState(), nullptr);
    }

    DOCTEST_CHECK(stateMachine.ChangeState(nullptr));
    DOCTEST_CHECK_FALSE(stateMachine.HasState());
    DOCTEST_CHECK_EQ(stateMachine.GetState(), nullptr);
}
