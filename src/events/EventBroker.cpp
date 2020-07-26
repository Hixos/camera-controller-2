/* Copyright (c) 2015-2020 Skyward Experimental Rocketry
 * Authors: Luca Erbetta, Matteo Michele Piazzolla
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

#include "EventBroker.h"
#include "utils/logger.h"

using std::unique_lock;

EventBroker::EventBroker() {}

void EventBroker::post(const shared_ptr<const Event>& ev, uint8_t topic)
{
    lock_guard<mutex> lock(mtx_subscribers);
    // Log.d("[EventBroker] Posting event %d on topic %d", ev->sig, topic);
    if (subscribers.count(topic) > 0)
    {
        vector<EventHandlerBase*>& subs = subscribers.at(topic);
        auto begin                      = subs.begin();
        auto end                        = subs.end();

        for (auto it = begin; it != end; it++)
        {
            // TODO: This may cause a deadlock if subscribe(...) in called in
            // postEvent(...), but it should never happen anyway. What to do?
            (*it)->postEvent(ev);
        }
    }
}

uint16_t EventBroker::postDelayed(const EventPtr& ev,
                                  uint8_t topic, unsigned int delay_ms)
{
    unique_lock<mutex> lock(mtx_delayed_events);

    uint16_t sched_id = eventCounter++;
    auto deadline     = system_clock::now() + milliseconds(delay_ms);
    // DelayedEvent dev{eventCounter++, ev, topic, getTick() + delay_ticks};
    bool added = false;

    // Add the new event in the list, ordered by deadline (first = nearest
    // deadline)
    for (auto it = delayed_events.begin(); it != delayed_events.end(); it++)
    {
        if (deadline < (*it).deadline)
        {
            delayed_events.emplace(it, sched_id, ev, topic, deadline);
            added = true;
            break;
        }
    }

    if (!added)  // In case this is the last/only event in the list
    {
        delayed_events.emplace_back(sched_id, ev, topic, deadline);
    }

    lock.unlock();
    cv_delayed_events.notify_one();

    return sched_id;
}

void EventBroker::removeDelayed(uint16_t id)
{
    lock_guard<mutex> lock(mtx_delayed_events);
    for (auto it = delayed_events.begin(); it != delayed_events.end(); it++)
    {
        if ((*it).sched_id == id)
        {
            delayed_events.erase(it);
            break;
        }
    }
}

void EventBroker::run()
{
    unique_lock<mutex> lock(mtx_delayed_events);
    while (!shouldStop())
    {
        if (delayed_events.size() > 0)
        {
             // Wait until the first deadline expires
            cv_delayed_events.wait_until(lock, delayed_events.front().deadline);
        }
        else
        {
            // Wait until a delayed event is added
            cv_delayed_events.wait(lock);
        }

        if (delayed_events.size() > 0)
        {
            // Check if the deadline has expired
            if (delayed_events.front().deadline <= system_clock::now())
            {
                DelayedEvent dev = delayed_events.front();
                delayed_events.erase(delayed_events.begin());

                // Unlock the mutex to avoid a deadlock if someone calls
                // postDelayed while receiving the event.
                lock.unlock();
                post(dev.event, dev.topic);

                // Lock it back
                lock.lock();
            }
        }
    }
}

void EventBroker::subscribe(EventHandlerBase* subscriber, uint8_t topic)
{
    lock_guard<mutex> lock(mtx_subscribers);
    subscribers[topic].push_back(subscriber);
}

void EventBroker::unsubscribe(EventHandlerBase* subscriber, uint8_t topic)
{
    lock_guard<mutex> lock(mtx_subscribers);

    deleteSubscriber(subscribers.at(topic), subscriber);
}

void EventBroker::unsubscribe(EventHandlerBase* subscriber)
{
    lock_guard<mutex> lock(mtx_subscribers);
    for (auto it = subscribers.begin(); it != subscribers.end(); it++)
    {
        deleteSubscriber(it->second, subscriber);
    }
}

void EventBroker::deleteSubscriber(vector<EventHandlerBase*>& subs,
                                   EventHandlerBase* subscriber)
{
    auto it = subs.begin();

    while (it != subs.end())
    {
        if (*it == subscriber)
        {
            it = subs.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void EventBroker::clearDelayedEvents()
{
    lock_guard<mutex> lock(mtx_delayed_events);
    delayed_events.clear();
}
