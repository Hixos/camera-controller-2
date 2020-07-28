/**
 * Copyright (c) 2020
 * Authors: Luca Erbetta (luca.erbetta105@gmail.com)
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

#include "EventLogger.h"

#include <functional>
#include <memory>

#include "cameracontroller/Events.h"
#include "utils/logger.h"

using namespace std;

void onEventReceived(const EventPtr& ev, uint8_t topic)
{
    switch (ev->sig)
    {
        case EV_CAMERA_CONNECTED:
        {
            auto cev = std::static_pointer_cast<const CameraConnectedEvent>(ev);

            Log.d("(%s) %s (%s)", getTopicName(topic).c_str(),
                  getEventName(ev->sig).c_str(), cev->serial.c_str());
            break;
        }
        case EV_CAMERA_CONNECTION_ERROR:
        case EV_CAMERA_GENERIC_ERROR:
        {
            auto cev = std::static_pointer_cast<const CameraErrorEvent>(ev);

            Log.d("(%s) %s Error %d: %s", getTopicName(topic).c_str(),
                  getEventName(ev->sig).c_str(), cev->errorcode,
                  cev->what().c_str());
            break;
        }
        case EV_CAMERA_VAL_STATUS:
        {
            auto sev = std::static_pointer_cast<const CameraStatusEvent>(ev);

            Log.d("(%s) %s (battery=%d, program='%s', focal_length=%d)",
                  getTopicName(topic).c_str(), getEventName(ev->sig).c_str(),
                  sev->status.battery, sev->status.exposure_program.c_str(),
                  sev->status.focal_length);
            break;
        }
        case EV_CAMERA_VAL_CONFIG:
        {
            using namespace gphotow;
            using CSC = CameraStringConversion;

            auto cev = std::static_pointer_cast<const CameraConfigEvent>(ev);

            Log.d("(%s) %s (shutter=%s, aperture='%s', iso=%d, trigger='%s')",
                  getTopicName(topic).c_str(), getEventName(ev->sig).c_str(),
                  CSC::shutterSpeedToString(cev->config.shutter_speed).c_str(),
                  CSC::apertureToString(cev->config.aperture).c_str(),
                  cev->config.iso, cev->config.trigger_mode.c_str());
            break;
        }
        case EV_CAMERA_OPTIONS_SHUTTER_SPEED:
        case EV_CAMERA_OPTIONS_APERTURE:
        case EV_CAMERA_OPTIONS_ISO:
        {
            using namespace gphotow;
            using CSC = CameraStringConversion;
            stringstream ss;

            function<string(int)> to_s;

            switch (ev->sig)
            {
                case EV_CAMERA_OPTIONS_SHUTTER_SPEED:
                    to_s = &CSC::shutterSpeedToString;
                    break;
                case EV_CAMERA_OPTIONS_APERTURE:
                    to_s = &CSC::apertureToString;
                    break;
                default:
                    to_s = static_cast<string (*)(int)>(&to_string);
                    break;
            }

            auto oev =
                std::static_pointer_cast<const CameraOptionListEvent<int>>(ev);

            for (auto it = oev->options.begin(); it != oev->options.end(); it++)
            {
                ss << "'" << to_s(*it) << "'";
                if (it != oev->options.end() - 1)
                {
                    ss << ", ";
                }
            }
            Log.d("(%s) %s {%s}", getTopicName(topic).c_str(),
                  getEventName(ev->sig).c_str(), ss.str().c_str());
            break;
        }
        case EV_CAMERA_OPTIONS_TRIGGER_MODE:
        {
            stringstream ss;

            auto oev =
                std::static_pointer_cast<const CameraOptionListEvent<string>>(
                    ev);

            for (auto it = oev->options.begin(); it != oev->options.end(); it++)
            {
                ss << "'" << *it << "'";
                if (it != oev->options.end() - 1)
                {
                    ss << ", ";
                }
            }
            Log.d("(%s) %s {%s}", getTopicName(topic).c_str(),
                  getEventName(ev->sig).c_str(), ss.str().c_str());
            break;
        }
        case EV_CAMERA_CAPTURE_SUCCESS:
        {
            auto e = static_pointer_cast<const CameraCaptureSuccessEvent>(ev);
            Log.d("(%s) %s (%s)", getTopicName(topic).c_str(),
                  getEventName(ev->sig).c_str(), e->path.getPath().c_str());
            break;
        }
        case EV_CAMERA_DOWNLOAD:
        {
            auto e = static_pointer_cast<const CameraDownloadEvent>(ev);
            Log.d("(%s) %s (%s)", getTopicName(topic).c_str(),
                  getEventName(ev->sig).c_str(),
                  e->camera_path.getPath().c_str());
            break;
        }
        case EV_CAMERA_DOWNLOAD_SUCCESS:
        {
            auto e = static_pointer_cast<const CameraDownloadSuccessEvent>(ev);
            Log.d("(%s) %s (%s -> %s)", getTopicName(topic).c_str(),
                  getEventName(ev->sig).c_str(),
                  e->camera_path.getPath().c_str(), e->path.c_str());
            break;
        }
        default:
            Log.d("(%s) %s", getTopicName(topic).c_str(),
                  getEventName(ev->sig).c_str());
            break;
    }
}