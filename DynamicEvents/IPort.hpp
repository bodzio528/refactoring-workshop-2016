#pragma once

#include <memory>

class Event;

class IPort
{
public:
    virtual ~IPort() = default;
    virtual void send(std::unique_ptr<Event>) = 0;
};
