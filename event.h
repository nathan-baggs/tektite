#pragma once

#include <cstdint>

enum class EventType
{
    KEY_DOWN,
    KEY_UP,
    MOUSE_MOVE
};

struct MouseMoveEvent
{
    float delta_x;
    float delta_y;
};

struct Event
{
    EventType type;

    union Data
    {
        std::uint8_t key;
        MouseMoveEvent mouse_move;
    } data;
};
