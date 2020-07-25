#include "Event.h"

using std::make_shared;

const EventPtr C_EV_ENTRY = make_shared<const Event>(Event(EV_ENTRY));
const EventPtr C_EV_EXIT = make_shared<const Event>(Event(EV_EXIT));
const EventPtr C_EV_EMPTY = make_shared<const Event>(Event(EV_EMPTY));
const EventPtr C_EV_INIT = make_shared<const Event>(Event(EV_INIT));