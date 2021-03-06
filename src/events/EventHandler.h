/* Event Handler
 *
 * Copyright (c) 2015-2018 Skyward Experimental Rocketry
 * Author: Luca Erbetta
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <memory>

#include "Event.h"
#include "async/ActiveObject.h"
#include "collections/SyncQueue.h"

using std::make_shared;
using std::shared_ptr;

class EventHandlerBase
{
public:
    EventHandlerBase() {}

    virtual ~EventHandlerBase(){};

    virtual void postEvent(const EventPtr& ev) = 0;
};

class EventHandler : public EventHandlerBase, public ActiveObject
{
public:
    EventHandler() : ActiveObject() {}

    virtual ~EventHandler(){};

    virtual void postEvent(const EventPtr& ev) override
    {
        eventList.put(ev);
    }

    virtual void stop() override
    {
        if (started && !stopped)
        {
            should_stop = true;
            eventList.put(make_shared<const Event>(Event{.sig=EV_EMPTY}));
            if (thread_obj->joinable())
                thread_obj->join();
            stopped = true;
        }
    }

protected:
    virtual void handleEvent(const EventPtr&) = 0;

    void run() override
    {
        while (!shouldStop())
        {
            EventPtr e = eventList.get();
            handleEvent(e);
        }
    }

    SynchronizedQueue<EventPtr> eventList;
};
