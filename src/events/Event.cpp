#include "Event.h"

using std::make_shared;

const shared_ptr<const Event> C_EV_ENTRY = make_shared<const Event>(EV_ENTRY);
const shared_ptr<const Event> C_EV_EXIT = make_shared<const Event>(EV_EXIT);
const shared_ptr<const Event> C_EV_EMPTY = make_shared<const Event>(EV_EMPTY);
const shared_ptr<const Event> C_EV_INIT = make_shared<const Event>(EV_INIT);