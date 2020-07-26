/**
 * Copyright (c) 2019 Skyward Experimental Rocketry
 * Authors: Luca Erbetta
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

#include <iostream>
#include <sstream>
#include <string>
#include "async/ActiveObject.h"
#include "events/EventBroker.h"
#include "cameracontroller/Events.h"

using std::string;
using std::stringstream;

class EventInjector : public ActiveObject
{
public:
protected:
    void run() override
    {
        using namespace std;

        uint16_t ev, topic;
        while(!shouldStop())
        {
            cout << "Insert Event & Topic:\n";

            string temp;
            getline(cin, temp);
            stringstream(temp) >> ev >> topic;

            switch (ev)
            {
                case 0:
                {
                    should_stop = true;
                    break;
                }
                case EV_CAMERA_SET_CONFIG:
                {
                    cout << "Insert shutter_speed aperture iso\n";
                    CameraConfig cfg;
                    string temp;
                    getline(cin, temp);
                    stringstream(temp) >> cfg.shutter_speed >> cfg.aperture >> cfg.iso;

                    EventPtr evptr = getEvPointer(CameraConfigEvent(ev, cfg));
                    sEventBroker.post(evptr, topic);
                    break;
                }
                default:
                {
                    sEventBroker.post(make_shared<const Event>(Event(ev)), topic);
                    break;
                }
            }
        }
    }

private:
};
