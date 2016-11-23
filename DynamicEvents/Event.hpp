#pragma once

#include <cstdint>
#include <memory>

struct Event
{
    virtual ~Event() = default;

    virtual std::uint32_t getMessageId() const = 0;
    virtual std::unique_ptr<Event> clone() const  = 0;
};
