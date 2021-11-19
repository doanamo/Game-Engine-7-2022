/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <string>
#include <memory>
#include <Common/StateMachine.hpp>
#include <gtest/gtest.h>

/*
    Helpers
*/

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

/*
    State Machine
*/

class StateMachine : public testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_FALSE(stateMachine.HasState());
        ASSERT_EQ(stateMachine.GetState(), nullptr);
    }

    void TearDown() override
    {
        EXPECT_TRUE(stateMachine.ChangeState(nullptr));
        EXPECT_FALSE(stateMachine.HasState());
        EXPECT_EQ(stateMachine.GetState(), nullptr);
    }

protected:
    Common::StateMachine<TestState> stateMachine;
};

TEST_F(StateMachine, RegularTransition)
{
    // Create states.
    int expectedCounterOne = 0;
    auto stateOne = std::make_shared<TestState>("One", 100);
    EXPECT_FALSE(stateOne->HasStateMachine());
    EXPECT_EQ(stateOne->GetStateMachine(), nullptr);
    EXPECT_EQ(stateOne->counter, expectedCounterOne += 100);

    int expectedCounterTwo = 0;
    auto stateTwo = std::make_shared<TestState>("Two", 200);
    EXPECT_FALSE(stateTwo->HasStateMachine());
    EXPECT_EQ(stateTwo->GetStateMachine(), nullptr);
    EXPECT_EQ(stateTwo->counter, expectedCounterTwo += 200);

    // Change to state one.
    EXPECT_TRUE(stateMachine.ChangeState(stateOne));
    EXPECT_TRUE(stateMachine.HasState());
    EXPECT_EQ(stateMachine.GetState(), stateOne);

    EXPECT_TRUE(stateOne->HasStateMachine());
    EXPECT_EQ(stateOne->GetStateMachine(), &stateMachine);
    EXPECT_EQ(stateOne->counter, expectedCounterOne += 10);

    // Change current state.
    EXPECT_TRUE(stateMachine.ChangeState(stateTwo));
    EXPECT_TRUE(stateMachine.HasState());
    EXPECT_EQ(stateMachine.GetState(), stateTwo);

    EXPECT_FALSE(stateOne->HasStateMachine());
    EXPECT_EQ(stateOne->GetStateMachine(), nullptr);
    EXPECT_EQ(stateOne->counter, expectedCounterOne -= 1);

    EXPECT_TRUE(stateTwo->HasStateMachine());
    EXPECT_EQ(stateTwo->GetStateMachine(), &stateMachine);
    EXPECT_EQ(stateTwo->counter, expectedCounterTwo += 10);

    // Change to previous state.
    EXPECT_TRUE(stateMachine.ChangeState(stateOne));
    EXPECT_TRUE(stateMachine.HasState());
    EXPECT_EQ(stateMachine.GetState(), stateOne);

    EXPECT_TRUE(stateOne->HasStateMachine());
    EXPECT_EQ(stateOne->GetStateMachine(), &stateMachine);
    EXPECT_EQ(stateOne->counter, expectedCounterOne += 10);

    EXPECT_FALSE(stateTwo->HasStateMachine());
    EXPECT_EQ(stateTwo->GetStateMachine(), nullptr);
    EXPECT_EQ(stateTwo->counter, expectedCounterTwo -= 1);

    // Change to blocked enter transition.
    stateTwo->allowEnter = false;

    EXPECT_FALSE(stateMachine.ChangeState(stateTwo));
    EXPECT_TRUE(stateMachine.HasState());
    EXPECT_EQ(stateMachine.GetState(), stateOne);

    EXPECT_TRUE(stateOne->HasStateMachine());
    EXPECT_EQ(stateOne->GetStateMachine(), &stateMachine);
    EXPECT_EQ(stateOne->counter, expectedCounterOne);

    EXPECT_FALSE(stateTwo->HasStateMachine());
    EXPECT_EQ(stateTwo->GetStateMachine(), nullptr);
    EXPECT_EQ(stateTwo->counter, expectedCounterTwo);

    stateTwo->allowEnter = true;

    // Change to blocked exit transition.
    stateOne->allowExit = false;

    EXPECT_FALSE(stateMachine.ChangeState(stateTwo));
    EXPECT_TRUE(stateMachine.HasState());
    EXPECT_EQ(stateMachine.GetState(), stateOne);

    EXPECT_TRUE(stateOne->HasStateMachine());
    EXPECT_EQ(stateOne->GetStateMachine(), &stateMachine);
    EXPECT_EQ(stateOne->counter, expectedCounterOne);

    EXPECT_FALSE(stateTwo->HasStateMachine());
    EXPECT_EQ(stateTwo->GetStateMachine(), nullptr);
    EXPECT_EQ(stateTwo->counter, expectedCounterTwo);

    stateOne->allowExit = true;

    // Check state after freeing shared pointers.
    stateOne = nullptr;
    stateTwo = nullptr;

    EXPECT_TRUE(stateMachine.HasState());
    stateOne = stateMachine.GetState();

    EXPECT_TRUE(stateOne->HasStateMachine());
    EXPECT_EQ(stateOne->GetStateMachine(), &stateMachine);
    EXPECT_EQ(stateOne->name, "One");
    EXPECT_EQ(stateOne->counter, expectedCounterOne);

    // Discard current state.
    stateMachine.ChangeState(nullptr);
    EXPECT_FALSE(stateMachine.HasState());
    EXPECT_EQ(stateMachine.GetState(), nullptr);

    EXPECT_FALSE(stateOne->HasStateMachine());
    EXPECT_EQ(stateOne->GetStateMachine(), nullptr);
    EXPECT_EQ(stateOne->counter, expectedCounterOne -= 1);
}

TEST_F(StateMachine, ChainedTransitions)
{
    // Create state machine.
    Common::StateMachine<TestState> stateMachine;
    EXPECT_FALSE(stateMachine.HasState());
    EXPECT_EQ(stateMachine.GetState(), nullptr);

    // Setup transition chain.
    auto stateFirst = std::make_shared<TestState>("First", 100);
    EXPECT_FALSE(stateFirst->HasStateMachine());
    EXPECT_EQ(stateFirst->GetStateMachine(), nullptr);

    auto stateSecond = std::make_shared<TestState>("Second", 200);
    EXPECT_FALSE(stateSecond->HasStateMachine());
    EXPECT_EQ(stateSecond->GetStateMachine(), nullptr);

    auto stateThird = std::make_shared<TestState>("Third", 300);
    EXPECT_FALSE(stateThird->HasStateMachine());
    EXPECT_EQ(stateThird->GetStateMachine(), nullptr);

    stateFirst->transitionOnEnter = stateSecond;
    stateSecond->transitionOnEnter = stateThird;
    stateThird->allowEnter = false;

    // Perform chained transitions.
    EXPECT_TRUE(stateMachine.ChangeState(stateFirst));
    EXPECT_TRUE(stateMachine.HasState());
    EXPECT_EQ(stateMachine.GetState(), stateSecond);

    EXPECT_FALSE(stateFirst->HasStateMachine());
    EXPECT_EQ(stateFirst->GetStateMachine(), nullptr);
    EXPECT_EQ(stateFirst->counter, 109);

    EXPECT_TRUE(stateSecond->HasStateMachine());
    EXPECT_EQ(stateSecond->GetStateMachine(), &stateMachine);
    EXPECT_EQ(stateSecond->counter, 210);

    EXPECT_FALSE(stateThird->HasStateMachine());
    EXPECT_EQ(stateThird->GetStateMachine(), nullptr);
    EXPECT_EQ(stateThird->counter, 300);
}

TEST_F(StateMachine, ExitTransitions)
{
    // Create state machine.
    Common::StateMachine<TestState> stateMachine;
    EXPECT_FALSE(stateMachine.HasState());
    EXPECT_EQ(stateMachine.GetState(), nullptr);

    // Create states.
    auto stateFirst = std::make_shared<TestState>();
    auto stateSecond = std::make_shared<TestState>();
    stateFirst->transitionOnExit = stateSecond;

    EXPECT_FALSE(stateFirst->HasStateMachine());
    EXPECT_EQ(stateFirst->GetStateMachine(), nullptr);

    EXPECT_FALSE(stateSecond->HasStateMachine());
    EXPECT_EQ(stateSecond->GetStateMachine(), nullptr);

    // Trigger transition on exit.
    EXPECT_TRUE(stateMachine.ChangeState(stateFirst));
    EXPECT_TRUE(stateMachine.HasState());
    EXPECT_EQ(stateMachine.GetState(), stateFirst);

    EXPECT_TRUE(stateMachine.ChangeState(nullptr));
    EXPECT_FALSE(stateMachine.HasState());
    EXPECT_EQ(stateMachine.GetState(), nullptr);
}
