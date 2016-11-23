#pragma once

#include <type_traits>

#include "Event.hpp"

template <class T>
class EventT : public Event
{
    static_assert(std::is_copy_constructible<T>::value, "Payload type must be copy-construcible!");
public:
    EventT(T const& payload = T())
        : m_payload(std::make_unique<T>(payload))
    {}

    EventT(T&& payload)
        : m_payload(std::make_unique<T>(std::forward<T>(payload)))
    {}

    EventT(EventT&&) = default;

    EventT(EventT<T> const&) = delete;
    EventT& operator=(EventT<T> const&) = delete;

    std::uint32_t getMessageId() const override { return T::MESSAGE_ID; };
    std::unique_ptr<Event> clone() const override { return std::make_unique<EventT<T>>(*m_payload); }

    T * const operator->() noexcept { return m_payload.get(); }
    T const * const operator->() const noexcept { return m_payload.get(); }

    T& operator*() noexcept { return *m_payload; }
    T const& operator*() const noexcept { return *m_payload; }

private:
    std::unique_ptr<T> m_payload;
};

template <class T>
T const& payload(Event const& p_evt)
{
    return *static_cast<EventT<T> const&>(p_evt);
}

template <class T>
T& payload(Event& p_evt)
{
    return *static_cast<EventT<T>&>(p_evt);
}
