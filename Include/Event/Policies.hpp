/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Event Policies
*/

namespace Event
{
    enum class SubscriptionPolicy
    {
        RetainSubscription,  // Keep existing subscription and fail subsequent subscription attempts.
        ReplaceSubscription, // Remove existing subscription to guarantee subsequent subscription attempts. 
    };

    enum class PriorityPolicy
    {
        InsertBack,  // Give normal priority by subscribing at the end of receiver list.
        InsertFront, // Give highest priority by subscribing at the beginning of receiver list.
    };
}
