#include <cstdint>

#include "events/Event.h"

#include <string>

using std::string;

enum CCEvents : uint8_t
{
    EV_CAMERA_CONNECT = EV_FIRST_SIGNAL,
    EV_CAMERA_DISCONNECT,
    EV_CAMERA_CONNECTED,
    EV_CAMERA_ERROR,
    EV_CAMERA_CAPTURE
};

struct CameraConnectedEvent : public Event
{
    string serial;

    CameraConnectedEvent(uint8_t sig, string serial)
        : Event(sig), serial(serial)
    {
    }
};