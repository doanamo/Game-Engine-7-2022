/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "System/Timer.hpp"

/*
    Update Timer
*/

namespace Game
{
    // Update timer class.
    class UpdateTimer : public System::Timer
    {
    public:
        UpdateTimer();
        ~UpdateTimer();

        // Disallow copying.
        UpdateTimer(const UpdateTimer& other) = delete;
        UpdateTimer& operator=(const UpdateTimer& other) = delete;

        // Move constructor and operator.
        UpdateTimer(UpdateTimer&& other);
        UpdateTimer& operator=(UpdateTimer&& other);

        // Initializes the update timer instance.
        bool Initialize() override;

        // Resets the timer.
        void Reset() override;

        // Updates timer by a given amount of time.
        // Returns true if an update could be performed.
        bool Update(float updateTime);

        // Gets the alpha time in normalized range between last two updates.
        float GetAlphaTime() const;

        // Gets the last successfully update time.
        float GetLastUpdateTime() const;

        // Gets the total update time in seconds.
        double GetTotalUpdateTime() const;

    private:
        // Update tracking values.
        uint64_t m_forwardUpdateCounter;
        uint64_t m_totalUpdateCounter;
        float m_lastUpdateTime;
    };
}
